#include "nodeopcda.h"

using namespace v8;

static uv_async_t s_async = { 0 };
std::string GetStr(VARIANT var);

class CMyCallback :public IAsynchDataCallback{
	WatchBaton* baton;
public:
	CMyCallback(WatchBaton * wbaton)
	{
		baton = wbaton;
	}

	void OnDataChange(COPCGroup & group, CAtlMap<COPCItem *, OPCItemData *> & datamap){

		POSITION pos = datamap.GetStartPosition();
		int index = 0;
		while (pos != NULL)
		{
			COPCItem* item = baton->itemsCreated[index++];
			OPCItemData* itemdata = datamap.GetNextValue(pos);
			std::string vNameStr = item->getName().c_str();
			std::string vDataStr = GetStr(itemdata->vDataValue);
			std::map<std::string, std::string>::iterator iter = baton->datacache.find(vNameStr);
			if (iter != baton->datacache.end())
			{
				if ((*iter).second != vDataStr){
					baton->dataChanged.insert(std::map<std::string, std::string>::value_type(vNameStr, vDataStr));
					(*iter).second = vDataStr;
				}
			}
		}
		s_async.data = baton;
		uv_async_send(&s_async);

		//printf("Group %s, item changes\n", group.getName().c_str());
		//POSITION pos = changes.GetStartPosition();
		//if (pos != NULL){
		//	OPCItemData* itemdata = changes.GetNextValue(pos);
		//	std::string vDataStr = GetStr(itemdata->vDataValue);
		//	//printf("async_cb called : %s !\n", vDataStr);

		//	char errorString[ERROR_STRING_SIZE];
		//	strcpy(errorString, "");
		//	strcpy(errorString, vDataStr.c_str());

		//	s_async.data = (void*)&errorString;
		//	//printf("s_async.data : %s !\n", errorString);
		//	uv_async_send(&s_async);
		//	//printf("-----> %s\n", item->getName().c_str());
		//	//OPCItemData data;
		//	//item->readSync(data, OPC_DS_DEVICE);
		//	//printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);
		//}

	}

};



void async_cb(uv_async_t* handle)
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

	//std::string* str = static_cast<std::string*>(handle->data);
	//printf("async_cb called : %s !\n", handle->data);
	//printf("async_cb called : %s !\n", *(std::string*)handle->data);
	//uv_thread_t id = uv_thread_self();
	//printf("thread id:%lu.\n", id);
	//uv_close((uv_handle_t*)&s_async, NULL);   //如果async没有关闭，消息队列是会阻塞的  
}

std::string GetStr(VARIANT var){
	CString str;
	//以下代码演示如何转换为C标准字符串型
	switch (var.vt)
	{
	case VT_EMPTY:{
		str = "";
		break;
	}
	case VT_BSTR:
	{
		str = var.bstrVal;
		break;
	}
	case VT_I2: //var is short int type 
	{
		str.Format("%d", (int)var.iVal);
		break;
	}
	case VT_I4: //var is long int type
	{
		str.Format("%d", var.lVal);
		break;
	}
	case VT_R4: //var is float type
	{
		str.Format("%10.6f", (double)var.fltVal);
		break;
	}
	case VT_R8: //var is double type
	{
		str.Format("%10.6f", var.dblVal);
		break;
	}
	//case VT_CY: //var is CY type
	//{
	//	str = COleCurrency(var).Format();
	//	break;
	//}
	//case VT_DATE: //var is DATE type
	//{
	//	str = COleDateTime(var).Format();
	//	break;
	//}
	case VT_BOOL: //var is VARIANT_BOOL
	{
		str = (var.boolVal == 0) ? "FALSE" : "TRUE";
		break;
	}
	default:
	{
		str.Format("Unk type %d\n", var.vt);
		break;
	}
	}
	return str.GetString();
}

v8::Local<v8::Value> getValueFromObject(v8::Local<v8::Object> options, std::string key) {
	v8::Local<v8::String> v8str = Nan::New<v8::String>(key).ToLocalChecked();
	return Nan::Get(options, v8str).ToLocalChecked();
}

v8::Local<v8::String> getStringFromObj(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->ToString();
}

#define MESSAGEPUMPUNTIL(x)	while(!x){{MSG msg;while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}Sleep(1);}}

void sub_thread(void* arg)
{
	InitBaton* initBaton = static_cast<InitBaton*>(arg);

	COPCClient::init();

	COPCHost *host = COPCClient::makeHost(initBaton->HostName);
	COPCServer *opcServer = host->connectDAServer(initBaton->ProgId);

	unsigned long refreshRate;
	COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);

	// make several items
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
	for (int i = 0; i < itemsCreated.size(); i++)
	{
		baton->datacache.insert(std::map<std::string, std::string>::value_type(itemsCreated[i]->getName().c_str(), ""));
		//printf("%s", itemsCreated[i]->getName().c_str());
	}
	baton->dataCallback = initBaton->dataCallback;

	CMyCallback usrCallBack(baton);
	group->enableAsynch(usrCallBack);

	MESSAGEPUMPUNTIL(false)
	/*uv_thread_t id = uv_thread_self();
	printf("sub thread id:%lu.\n", id);
	uv_async_send(&s_async);*/
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
	//memset(initBaton, 0, sizeof(InitBaton));
	//printf("%s\n", std::string(*lHostName));
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

	uv_async_init(uv_default_loop(), &s_async, async_cb);
	//创建子线程  
	uv_thread_t thread;
	uv_thread_create(&thread, sub_thread, initBaton);


	//COPCClient::init();

	//COPCHost *host = COPCClient::makeHost(initBaton->HostName);
	//COPCServer *opcServer = host->connectDAServer(initBaton->ProgId);

	//unsigned long refreshRate;
	//COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);

	//// make several items
	//std::vector<COPCItem *>itemsCreated;
	//std::vector<HRESULT> errors;

	//if (group->addItems(initBaton->itemNames, itemsCreated, errors, true) != 0){
	//	printf("Item create failed\n");
	//}

	//CMyCallback usrCallBack;
	//group->enableAsynch(usrCallBack);


	//WatchBaton* baton = new WatchBaton();

	//strcpy(baton->errorString, "");
	//baton->opcServer = opcServer;
	//baton->group = group;
	//baton->itemsCreated = itemsCreated;
	//for (int i = 0; i < itemsCreated.size(); i++)
	//{
	//	baton->datacache.insert(std::map<std::string, std::string>::value_type(itemsCreated[i]->getName().c_str(), ""));
	//	//printf("%s", itemsCreated[i]->getName().c_str());
	//}
	//baton->dataCallback = initBaton->dataCallback;

	//Sleep(100000);

	//DataChangeNode(baton);

}


void DataChangeNode(WatchBaton* baton){
	uv_work_t * req = new uv_work_t();
	req->data = baton;
	uv_queue_work(uv_default_loop(), req, EIO_WatchData, (uv_after_work_cb)EIO_AfterWatchData);
}

void EIO_WatchData(uv_work_t* req){
	WatchBaton* baton = static_cast<WatchBaton*>(req->data);

	COPCItem_DataMap datamap;
	baton->group->readSync(baton->itemsCreated, datamap, OPC_DS_CACHE);

	POSITION pos = datamap.GetStartPosition();
	int index = 0;
	while (pos != NULL)
	{
		COPCItem* item = baton->itemsCreated[index++];
		OPCItemData* itemdata = datamap.GetNextValue(pos);
		std::string vNameStr = item->getName().c_str();
		std::string vDataStr = GetStr(itemdata->vDataValue);
		std::map<std::string, std::string>::iterator iter = baton->datacache.find(vNameStr);
		if (iter != baton->datacache.end())
		{
			if ((*iter).second != vDataStr){
				baton->dataChanged.insert(std::map<std::string, std::string>::value_type(vNameStr, vDataStr));
				(*iter).second = vDataStr;
			}
		}
	}
}

void EIO_AfterWatchData(uv_work_t* req){
	Nan::HandleScope scope;
	WatchBaton* baton = static_cast<WatchBaton*>(req->data);

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

	DataChangeNode(baton);
}


void InterfaceInit(v8::Handle<v8::Object> target) {
	Nan::HandleScope scope;
	Nan::SetMethod(target, "Init", Init);
}


NODE_MODULE(opcda, InterfaceInit)



