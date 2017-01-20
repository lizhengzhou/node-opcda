#include "nodeopcda.h"

using namespace v8;


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

	DataChangeNode(baton);

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



