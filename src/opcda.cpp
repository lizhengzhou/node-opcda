// Copyright 2017 lizz <lizhenzhou1026@gmail.com>

#include "nodeopcda.h"

using namespace v8;
static std::vector<COPCItem *>_itemsCreated;


class dataChangeCallback :public IAsynchDataCallback{
	WatchBaton* baton;
public:
	dataChangeCallback(WatchBaton * wbaton)
	{
		baton = wbaton;
	}

	void OnDataChange(COPCGroup & group, CAtlMap<COPCItem *, OPCItemData *> & datamap){

		POSITION pos = datamap.GetStartPosition();
		std::string vNameStr, vDataStr;
		while (pos != NULL)
		{
			COPCItem_DataMap::CPair* itemPair = datamap.GetNext(pos);
			
			COPCItem* item = itemPair->m_key;
			OPCItemData* itemdata = itemPair->m_value;

			//printf("Value=%s\n", GetStr(itemdata->vDataValue));
			vDataStr = GetStr(itemdata->vDataValue);

			for (int i = 0; i < baton->itemsCreated.size(); i++){
				if ((DWORD)baton->itemsCreated[i] == (DWORD)item)
				{
					item = baton->itemsCreated[i];
					//printf("find2\n");
				}
			}
			//printf("Name=%s\n", item->getName().c_str());
			vNameStr = item->getName().c_str();
			baton->dataChanged.insert(std::map<std::string, std::string>::value_type(vNameStr, vDataStr));			
		}
		s_async.data = baton;
		uv_async_send(&s_async);
	}
};


void async_dataCallback(uv_async_t* handle)
{
	Nan::HandleScope scope;
	WatchBaton* baton = static_cast<WatchBaton*>(handle->data);

	Local<Array> resultList = Nan::New<Array>();

	std::map<std::string, std::string>::iterator iter;
	int i = 0;
	for (iter = baton->dataChanged.begin(); iter != baton->dataChanged.end(); ++iter)
	{
		Local<Object> item = Nan::New<Object>();
		item->Set(Nan::New<String>("Addr").ToLocalChecked(), Nan::New<String>((*iter).first).ToLocalChecked());
		item->Set(Nan::New<String>("Value").ToLocalChecked(), Nan::New<String>((*iter).second).ToLocalChecked());
		resultList->Set(i, item);
		i++;
	}
	baton->dataChanged.clear();

	unsigned int argc = 2;
	v8::Local<v8::Value> argv[2];
	argv[0] = Nan::Undefined();
	argv[1] = resultList;

	if (resultList->Length() > 0 && baton->dataCallback){
		baton->dataCallback->Call(argc, argv);
	}

	//uv_close((uv_handle_t*)&s_async, NULL);   //如果async没有关闭，消息队列是会阻塞的  
}


void gather_thread(void* arg)
{
	InitBaton* initBaton = static_cast<InitBaton*>(arg);

	COPCClient::init();

	COPCHost *host = COPCClient::makeHost(initBaton->HostName);
	COPCServer *opcServer = host->connectDAServer(initBaton->ProgId);

	unsigned long refreshRate;
	COPCGroup *group = opcServer->makeGroup("47CFF29B-69D6-4FF7-8FFE-DF807EEEAB93", true, 1000, refreshRate, 0.0);
	
	std::vector<COPCItem *>itemsCreated;
	std::vector<HRESULT> errors;

	if (group->addItems(initBaton->itemNames, itemsCreated, errors, true) != 0){
		printf("Item create failed\n");
	}

	WatchBaton* baton = new WatchBaton();
	strcpy(baton->errorString, "");
	baton->opcServer = opcServer;
	baton->group = group;
	baton->itemsCreated = itemsCreated;
	_itemsCreated = itemsCreated;
	for (int i = 0; i < itemsCreated.size(); i++)
	{
		baton->datacache.insert(std::map<std::string, std::string>::value_type(itemsCreated[i]->getName().c_str(), ""));		
	}
	baton->dataCallback = initBaton->dataCallback;

	dataChangeCallback usrCallBack(baton);
	group->enableAsynch(usrCallBack);

	MESSAGEPUMPUNTIL(false)
}

NAN_METHOD(Init) {
	Nan::HandleScope scope;
	// host
	if (!info[0]->IsString()) {
		Nan::ThrowTypeError("First argument must be a string");
		return;
	}
	v8::String::Utf8Value lHostName(info[0]->ToString());

	// options
	if (!info[1]->IsObject()) {
		Nan::ThrowTypeError("Second argument must be an object");
		return;
	}
	v8::Local<v8::Object> lOptions = Local<Object>::Cast(info[1]);

	// callback
	if (!info[2]->IsFunction()) {
		Nan::ThrowTypeError("Third argument must be a function");
		return;
	}

	InitBaton* initBaton = new InitBaton();
	initBaton->HostName = std::string(*lHostName);
	
	v8::String::Utf8Value ProgId(getStringFromObj(lOptions, "ProgId"));
	initBaton->ProgId = std::string(*ProgId);

	Local<Array> itemNames = Local<Array>::Cast(getValueFromObject(lOptions, "itemNames"));
	for (unsigned int i = 0; i < itemNames->Length(); i++) {
		v8::String::Utf8Value itemName(itemNames->Get(i));
		//printf("%s\n", std::string(*itemName));
		initBaton->itemNames.push_back(std::string(*itemName));
	}
	initBaton->dataCallback = new Nan::Callback(getValueFromObject(lOptions, "OnDataChange").As<v8::Function>());
	initBaton->callback.Reset(info[2].As<v8::Function>());

	uv_async_init(uv_default_loop(), &s_async, async_dataCallback);

	uv_thread_t thread;
	uv_thread_create(&thread, gather_thread, initBaton);

}

void InterfaceInit(v8::Handle<v8::Object> target) {
	Nan::HandleScope scope;
	Nan::SetMethod(target, "Init", Init);
}


NODE_MODULE(opcda, InterfaceInit)



