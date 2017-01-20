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

NAN_METHOD(Init) {
	//Nan::HandleScope scope;

	COPCClient::init();
	std::string hostName = "DESKTOP-JFJ715E";
	COPCHost *host = COPCClient::makeHost(hostName);

	std::string progId = "Kepware.KEPServerEX.V6";
	COPCServer *opcServer = host->connectDAServer(progId);

	unsigned long refreshRate;
	COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);

	// make several items
	std::vector<std::string> itemNames;
	std::vector<COPCItem *>itemsCreated;
	std::vector<HRESULT> errors;

	itemNames.push_back("Data Type Examples.16 Bit Device.R Registers.Short1");
	itemNames.push_back("Data Type Examples.16 Bit Device.R Registers.Short2");
	if (group->addItems(itemNames, itemsCreated, errors, true) != 0){
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
	baton->dataCallback = new Nan::Callback(info[0].As<v8::Function>());

	DataChangeNode(baton);
	//args.GetReturnValue().SetUndefined();
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



