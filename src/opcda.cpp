#include "nodeopcda.h"
//#include <v8.h>

using namespace v8;
//
//void EIO_List(uv_work_t* req) {
//	ListBaton* data = static_cast<ListBaton*>(req->data);
//
//	COPCClient::init();
//	std::string hostName = data->hostName;
//	COPCHost *host = COPCClient::makeHost(hostName);
//	//List servers
//	std::vector<std::string> localServerList = data->ServerList;
//	host->getListOfDAServers(IID_CATID_OPCDAServer20, data->ServerList);
//}
//
//void EIO_AfterList(uv_work_t* req) {
//	Nan::HandleScope scope;
//
//	ListBaton* data = static_cast<ListBaton*>(req->data);
//
//	v8::Local<v8::Value> argv[2];
//	if (data->errorString[0]) {
//		argv[0] = v8::Exception::Error(Nan::New<v8::String>(data->errorString).ToLocalChecked());
//		argv[1] = Nan::Undefined();
//	}
//	else {
//		v8::Local<v8::Array> results = Nan::New<v8::Array>();
//		int i = 0;
//		for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it, i++) {
//			Nan::Set(results, i, Nan::New<v8::String>(*it).ToLocalChecked());
//		}
//		argv[0] = Nan::Null();
//		argv[1] = results;
//	}
//	data->callback.Call(2, argv);
//
//	//for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it) {
//	//	delete *it;
//	//}
//	delete data;
//	delete req;
//}
//
//
//
//NAN_METHOD(List) {
//	if (!info[0]->IsString()) {
//		Nan::ThrowTypeError("First argument must be a string");
//		return;
//	}
//	v8::String::Utf8Value host(info[0]->ToString());
//	// callback
//	if (!info[1]->IsFunction()) {
//		Nan::ThrowTypeError("Second argument must be a function");
//		return;
//	}
//
//	ListBaton* baton = new ListBaton();
//	strcpy(baton->hostName, *host);
//	strcpy(baton->errorString, "");
//	baton->callback.Reset(info[1].As<v8::Function>());
//
//	uv_work_t* req = new uv_work_t();
//	req->data = baton;
//	uv_queue_work(uv_default_loop(), req, EIO_List, (uv_after_work_cb)EIO_AfterList);
//
//	return;
//}
//
//// Convert a JavaScript string to a std::string. 
//// To not bother too much with string encodings 
////we just use ascii.
//std::string ObjectToString(Local<Value> value) {
//	String::Utf8Value utf8_value(value);
//	return std::string(*utf8_value);
//}
//
//void EIO_Init(uv_work_t* req) {
//	InitBatonOptions* data = static_cast<InitBatonOptions*>(req->data);
//
//	COPCClient::init();
//	std::string hostName = ObjectToString(data->hostName);
//	COPCHost *host = COPCClient::makeHost(hostName);
//
//	std::string progId = ObjectToString(data->progId);
//
//	COPCServer *opcServer = host->connectDAServer(progId);
//
//	// Check status
//	ServerStatus status;
//	opcServer->getStatus(status);
//	printf("Server state is %ld\n", status.dwServerState);
//
//	// browse server
//	std::vector<std::string> opcItemNames;
//	opcServer->getItemNames(opcItemNames);
//
//
//	// make group
//	unsigned long refreshRate;
//	COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);
//	
//	// make several items
//	std::vector<std::string> itemNames;
//	std::vector<COPCItem *>itemsCreated;
//	std::vector<HRESULT> errors;
//
//	itemNames.push_back(opcItemNames[15]);
//	itemNames.push_back(opcItemNames[16]);
//	if (group->addItems(itemNames, itemsCreated, errors, true) != 0){
//		printf("Item create failed\n");
//	}
//
//	//CMyCallback usrCallBack();
//	//group->enableAsynch(usrCallBack);
//
//	//List servers
//	//std::vector<std::string> localServerList = data->ServerList;
//	//host->getListOfDAServers(IID_CATID_OPCDAServer20, data->ServerList);
//}
//
//void EIO_AfterInit(uv_work_t* req) {
//	Nan::HandleScope scope;
//
//	ListBaton* data = static_cast<ListBaton*>(req->data);
//
//	v8::Local<v8::Value> argv[2];
//	if (data->errorString[0]) {
//		argv[0] = v8::Exception::Error(Nan::New<v8::String>(data->errorString).ToLocalChecked());
//		argv[1] = Nan::Undefined();
//	}
//	else {
//		v8::Local<v8::Array> results = Nan::New<v8::Array>();
//		int i = 0;
//		for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it, i++) {
//			Nan::Set(results, i, Nan::New<v8::String>(*it).ToLocalChecked());
//		}
//		argv[0] = Nan::Null();
//		argv[1] = results;
//	}
//	data->callback.Call(2, argv);
//
//	//for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it) {
//	//	delete *it;
//	//}
//	delete data;
//	delete req;
//}
//
////InitBatonOptions* ParseOptions(const v8::Local<v8::Object>& options) {
////	// currently none
////	return new InitBatonOptions();
////}
//
//v8::Local<v8::Value> getValueFromObject(v8::Local<v8::Object> options, std::string key) {
//	v8::Local<v8::String> v8str = Nan::New<v8::String>(key).ToLocalChecked();
//	return Nan::Get(options, v8str).ToLocalChecked();
//}
//
//int getIntFromObject(v8::Local<v8::Object> options, std::string key) {
//	return getValueFromObject(options, key)->ToInt32()->Int32Value();
//}
//
//bool getBoolFromObject(v8::Local<v8::Object> options, std::string key) {
//	return getValueFromObject(options, key)->ToBoolean()->BooleanValue();
//}
//
//v8::Local<v8::String> getStringFromObj(v8::Local<v8::Object> options, std::string key) {
//	return getValueFromObject(options, key)->ToString();
//}
//
//double getDoubleFromObject(v8::Local<v8::Object> options, std::string key) {
//	return getValueFromObject(options, key)->ToNumber()->NumberValue();
//}


//
//
//NAN_METHOD(Init) {
//	if (!info[0]->IsObject) {
//		Nan::ThrowTypeError("First argument must be Options");
//		return;
//	}
//	v8::Local<v8::Object> options = info[0]->ToObject();
//	// callback
//	if (!info[1]->IsFunction()) {
//		Nan::ThrowTypeError("Second argument must be a function");
//		return;
//	}
//
//	InitBatonOptions* baton = new InitBatonOptions();
//	memset(baton, 0, sizeof(InitBatonOptions));
//
//	baton->hostName = getStringFromObj(options, "hostName");
//	baton->progId = getStringFromObj(options, "progId");
//	baton->dataChange = new Nan::Callback(getValueFromObject(options, "dataChange").As<v8::Function>());
//
//	baton->errorString = Nan::New<v8::String>("").ToLocalChecked();
//	baton->callback.Reset(info[1].As<v8::Function>());
//
//	uv_work_t* req = new uv_work_t();
//	req->data = baton;
//	uv_queue_work(uv_default_loop(), req, EIO_Init, (uv_after_work_cb)EIO_AfterInit);
//
//	return;
//}


static short curValue;

/**
* Handle asynch data coming from changes in the OPC group
*/
class CMyCallback :public IAsynchDataCallback{
	//Nan::Callback* Callback;
public:
	//CMyCallback(){

	//}

	//CMyCallback(Nan::Callback* dataCallback){
	//	Callback = dataCallback;
	//}
	void OnDataChange(COPCGroup & group, CAtlMap<COPCItem *, OPCItemData *> & changes){
		//Isolate* isolate = Isolate::GetCurrent();
		printf("Group %s, item changes\n", group.getName().c_str());
		try{
			POSITION pos = changes.GetStartPosition();
			//printf("Group %d, item changes\n", pos);
			
			//while (pos != NULL)
			{
				COPCItem * item = changes.GetNextKey(pos);
				printf("-----> %s\n", item->getName().c_str());
				OPCItemData data;
				item->readSync(data, OPC_DS_DEVICE);
				printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);

				curValue = data.vDataValue.iVal;
				/*Nan::HandleScope scope;
				curValue.Reset(isolate,Nan::New(data.vDataValue.iVal));*/
				//curValue = ;

				//Nan::HandleScope scope;
				//const unsigned argc = 1;
				//Local<Value> argv[argc] = {Nan::New(data.vDataValue.iVal )};
				////Local<Function>::New(isolate, s_logCallback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);
				//Callback->Call(1, argv);
			}
		}
		catch (Exception ex){
			printf("-----> %s\n", ex.Error);
		}
		
/*
		printf("-----> begin\n");
		Isolate* isolate = Isolate::GetCurrent();
		const unsigned argc = 1;
		Local<Value> argv[argc] = { Nan::New<v8::String>("hello world1").ToLocalChecked() };
		Local<Function>::New(isolate, s_logCallback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);
		printf("-----> end\n");
*/

		
	}

};


#define MESSAGEPUMPUNTIL(x)	while(!x){{MSG msg;while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}Sleep(1);}}

void Init(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();	

	///*s_logCallback*/.Reset(isolate,Local<Function>::Cast( args[0]));
	

		COPCClient::init();
		std::string hostName = "DESKTOP-JFJ715E";
		COPCHost *host = COPCClient::makeHost(hostName);
	
		std::string progId = "Kepware.KEPServerEX.V6";
		COPCServer *opcServer = host->connectDAServer(progId);
	
		//// Check status
		//ServerStatus status;
		//opcServer->getStatus(status);
		//printf("Server state is %ld\n", status.dwServerState);
	
		// browse server
		std::vector<std::string> opcItemNames;
		opcServer->getItemNames(opcItemNames);
	
	
		// make group
		unsigned long refreshRate;
		COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);
		
		// make several items
		std::vector<std::string> itemNames;
		std::vector<COPCItem *>itemsCreated;
		std::vector<HRESULT> errors;
	
		itemNames.push_back("Data Type Examples.16 Bit Device.R Registers.Short1");
		//itemNames.push_back(opcItemNames[16]);
		if (group->addItems(itemNames, itemsCreated, errors, true) != 0){
			printf("Item create failed\n");
		}
	
	


		CMyCallback usrCallBack;// = CMyCallback(callback);
		group->enableAsynch(usrCallBack);

		//Isolate* isolate = Isolate::GetCurrent();

		//const unsigned argc = 1;
		//Local<Value> argv[argc] = { Nan::New<v8::String>("hello world").ToLocalChecked() };

		//Local<Function>::New(isolate, s_logCallback)->Call(isolate->GetCurrentContext()->Global(), argc, argv);


		
	/*	uv_loop_t idler;
		uv_loop_init(&idler);*/
		uv_work_t* req = new uv_work_t();

		//callback->Reset(args[0].As<v8::Function>());
		//MESSAGEPUMPUNTIL(false);
		Nan::Callback* callback = new Nan::Callback(args[0].As<v8::Function>());;
		DataChangeNode(opcServer, group,itemsCreated, callback);
		//printf("printf DataChangeNode(opcServer, callback); \n");
		//uv_queue_work(uv_default_loop(), req, Loop, (uv_after_work_cb)Loop);
		/*
		uv_idle_init(uv_default_loop(), &idler);
		uv_idle_start(&idler, nullptr);
*/

		args.GetReturnValue().SetUndefined();
}


void DataChangeNode(COPCServer *opcServer, COPCGroup *group, std::vector<COPCItem *>itemsCreated, Nan::Callback* dataCallback){

	//printf("printf DataChangeNode \n");

	DataBaton* baton = new DataBaton();
	memset(baton, 0, sizeof(DataBaton));
	baton->opcServer = opcServer;
	baton->group = group;
	baton->itemsCreated = itemsCreated;
	baton->dataCallback = dataCallback;
	//baton->data = Nan::EmptyString();

	uv_work_t * req = new uv_work_t();
	req->data = baton;

	uv_queue_work(uv_default_loop(), req, EIO_Data, (uv_after_work_cb)EIO_AfterData);

}


static short cacheData;

void EIO_Data(uv_work_t* req){
	DataBaton* data = static_cast<DataBaton*>(req->data);
	
	//COPCServer* opcServer = data->opcServer;
	//ServerStatus status;
	//opcServer->getStatus(status);

	COPCGroup* group = data->group;

	COPCItem_DataMap datamap;
	group->readSync(data->itemsCreated, datamap, OPC_DS_CACHE);
	//data->dataMap = datamap;
	POSITION pos = datamap.GetStartPosition();
	//OPCItemData* itemdata = datamap.GetNextValue(pos);
	CAtlMap<COPCItem *, OPCItemData *>::CPair* firstMap = datamap.GetNext(pos);


	COPCItem* item = firstMap->m_key;
	OPCItemData* itemdata = firstMap->m_value;
//std::vector<COPCItem*>::iterator result = find(data->itemsCreated.begin(), data->itemsCreated.end(), item);
//if (result == data->itemsCreated.end()) //Ã»ÕÒµ½
//printf("%s", "ok");
//else //ÕÒµ½
//printf("%s", "none");

data->dataChanged = false;
if (cacheData != itemdata->vDataValue.iVal)
{
	cacheData = itemdata->vDataValue.iVal;
	data->dataChanged = true;
}
//(*result)->
	//printf("Server state is %ld\n", status.dwServerState);

	//short curValue = 100;
	itoa(itemdata->vDataValue.iVal, data->Str, 10);
	//printf("%s", data->Str);
	//strcpy(data->Str, itoa(curValue, data->Str,0));
	//Isolate* isolate = Isolate::GetCurrent();
	//data->data = curValue.Get(isolate)->ToString();

	//printf("printf Hello \n");

}

void EIO_AfterData(uv_work_t* req){

	//printf("printf EIO_AfterData \n");

	Nan::HandleScope scope;

	DataBaton* data = static_cast<DataBaton*>(req->data);
	//Isolate* isolate = Isolate::GetCurrent();
	v8::Local<v8::Value> argv[1];
	argv[0] = Nan::New<String>(data->Str).ToLocalChecked();
	
	if (data->dataChanged){
		data->dataCallback->Call(1, argv);
	}
	

	DataChangeNode(data->opcServer,data->group,data->itemsCreated, data->dataCallback);
}




void InterfaceInit(Local<Object> exports) {
	NODE_SET_METHOD(exports, "hello", Init);
}


NODE_MODULE(opcda, InterfaceInit)



