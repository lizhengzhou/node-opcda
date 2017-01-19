#include "nodeopcda.h"

using namespace v8;


std::string GetStr(VARIANT var){
	CString str;
	//以下代码演示如何转换为C标准字符串型
	switch (var.vt)
	{
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


/**
* Handle asynch data coming from changes in the OPC group
*/
class CMyCallback :public IAsynchDataCallback{
public:
	void OnDataChange(COPCGroup & group, CAtlMap<COPCItem *, OPCItemData *> & changes){
		//Isolate* isolate = Isolate::GetCurrent();
		printf("Group %s, item changes\n", group.getName().c_str());
			POSITION pos = changes.GetStartPosition();

			{
				COPCItem * item = changes.GetNextKey(pos);
				printf("-----> %s\n", item->getName().c_str());
				OPCItemData data;
				item->readSync(data, OPC_DS_DEVICE);
				printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);

			}
	}
};

void Init(const FunctionCallbackInfo<Value>& args) {
	Nan::HandleScope scope;

	COPCClient::init();
	std::string hostName = "DESKTOP-JFJ715E";
	COPCHost *host = COPCClient::makeHost(hostName);

	std::string progId = "Kepware.KEPServerEX.V6";
	COPCServer *opcServer = host->connectDAServer(progId);

	//// browse server
	//std::vector<std::string> opcItemNames;
	//opcServer->getItemNames(opcItemNames);

	// make group
	unsigned long refreshRate;
	COPCGroup *group = opcServer->makeGroup("Group", true, 1000, refreshRate, 0.0);

	// make several items
	std::vector<std::string> itemNames;
	std::vector<COPCItem *>itemsCreated;
	std::vector<HRESULT> errors;

	itemNames.push_back("Data Type Examples.16 Bit Device.R Registers.Short1");
	if (group->addItems(itemNames, itemsCreated, errors, true) != 0){
		printf("Item create failed\n");
	}

	//OPCItemData data;
	//itemsCreated[0]->readSync(data, OPC_DS_DEVICE);
	//printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);

	WatchBaton* baton = new WatchBaton();
	//memset(baton, 0, sizeof(WatchBaton));
	strcpy(baton->errorString, "");
	baton->opcServer = opcServer;
	baton->group = group;
	baton->itemsCreated = itemsCreated;		
	for (int i = 0; i < itemsCreated.size(); i++)
	{				
		baton->datacache.insert(std::map<std::string, std::string>::value_type(itemsCreated[i]->getName().c_str(), ""));
		//printf("%s", itemsCreated[i]->getName().c_str());
	}
	baton->dataCallback = new Nan::Callback(args[0].As<v8::Function>());	

	DataChangeNode(baton);	
	args.GetReturnValue().SetUndefined();
}


void DataChangeNode(WatchBaton* baton){
	uv_work_t * req = new uv_work_t();
	req->data = baton;
	uv_queue_work(uv_default_loop(), req, EIO_WatchData, (uv_after_work_cb)EIO_AfterWatchData);	
}

void EIO_WatchData(uv_work_t* req){
	WatchBaton* baton = static_cast<WatchBaton*>(req->data);
	COPCGroup* group = baton->group;
	COPCItem_DataMap datamap;
	group->readSync(baton->itemsCreated, datamap, OPC_DS_CACHE);
	printf("%s\n", baton->itemsCreated[0]->getName().c_str());

	OPCItemData data;
	ServerStatus status;
	baton->opcServer->getStatus(status);
	printf("Server state is %ld\n", status.dwServerState);
	printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);
	try{
		baton->itemsCreated[0]->readSync(data, OPC_DS_DEVICE);
	}
	catch (const char* &e){
		std::cout << "Standard exception: " << e << std::endl;
	}
	
	printf("Synch read quality %d value %d\n", data.wQuality, data.vDataValue.iVal);

	POSITION pos = datamap.GetStartPosition();
	while (pos != NULL)
	{
		CAtlMap<COPCItem *, OPCItemData *>::CPair* mapitem = datamap.GetNext(pos);
		printf("%s\n", "hello word");
		COPCItem* item = mapitem->m_key;
		OPCItemData* itemdata = mapitem->m_value;

		//printf("%s\n", item->getName().c_str());

		//printf("%s\n", item->getName().c_str());
		//std::string vNameStr = item->getName().c_str();
		printf("%d\n", itemdata->vDataValue.iVal);
		//char str[1024];
		//itoa(itemdata->vDataValue.iVal, str, 10);
		//std::string vDataStr = str;
		//printf("%s\n", "hello word");
		//std::map<std::string, std::string>::iterator iter = baton->datacache.find(vNameStr);
		//if (iter != baton->datacache.end())
		//{
		//	printf("%s\n", "hello word");
		//	if ((*iter).second != vDataStr){
		//		baton->dataChanged.insert(std::map<std::string, std::string>::value_type(vNameStr, vDataStr));
		//		(*iter).second = vDataStr;
		//	}
		//}
	}
	printf("%s\n", "end");

	//itoa(itemdata->vDataValue.iVal, data->Str, 10);
}

void EIO_AfterWatchData(uv_work_t* req){
	printf("%s\n", "EIO_AfterWatchData");
	Nan::HandleScope scope;
	WatchBaton* baton = static_cast<WatchBaton*>(req->data);

	Local<Array> resultList = Nan::New<Array>();

	std::map<std::string, std::string>::iterator iter;
	int i = 0;
	for (iter = baton->dataChanged.begin(); iter != baton->dataChanged.end(); ++iter)
	{		
		Local<Object> item = Nan::New<Object>();
		item->Set(Nan::New<String>("Addr").ToLocalChecked(), Nan::New<String>((*iter).first).ToLocalChecked());
		resultList->Set(i, item);
		i++;
	}
	unsigned int argc = 2;
	v8::Local<v8::Value> argv[2];
	argv[0] = Nan::Undefined();
	argv[1] = resultList;

	if (baton->dataCallback){
		baton->dataCallback->Call(argc, argv);
	}

	//DataChangeNode(baton);
}


void InterfaceInit(Local<Object> exports) {
	NODE_SET_METHOD(exports, "OnDataChange", Init);
}


NODE_MODULE(opcda, InterfaceInit)



