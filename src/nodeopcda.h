// Copyright 2017 lizz <lizhenzhou1026@gmail.com>

#ifndef SRC_NODE_OPCDA_H_
#define SRC_NODE_OPCDA_H_


#include "OPCClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <string>
#include "nan\nan.h"
#include "opcda.h"
#include "OPCHost.h"
#include "OPCServer.h"
#include "OPCGroup.h"
#include "OPCItem.h"
#include <sys\timeb.h>
#include <vector>
#include <algorithm>
#include <map>
#include <COMUTIL.H>

#pragma comment(lib, "comsupp.lib")

#define ERROR_STRING_SIZE 1024
#define MESSAGEPUMPUNTIL(x)	while(!x){{MSG msg;while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)){TranslateMessage(&msg);DispatchMessage(&msg);}Sleep(1);}}

struct InitBaton
{
	std::string HostName;
	std::string ProgId;
	std::vector<std::string>itemNames;
	Nan::Callback* dataCallback;
	Nan::Callback callback;
	char errorString[ERROR_STRING_SIZE];
};

struct WatchBaton
{
	COPCServer *opcServer;
	COPCGroup *group;
	std::vector<COPCItem *>itemsCreated;
	std::map<std::string, std::string> datacache;
	std::map<std::string, std::string> dataChanged;
	Nan::Callback* dataCallback;
	Nan::Callback callback;
	char errorString[ERROR_STRING_SIZE];
};

NAN_METHOD(Init);
//void DataChangeNode(WatchBaton* baton);
//void EIO_WatchData(uv_work_t* req);
//void EIO_AfterWatchData(uv_work_t* req);

std::string GetStr(VARIANT var){
	CString str;
	_variant_t temp(var);
	temp.ChangeType(VT_BSTR);
	str = temp.bstrVal;
	return str.GetString();

	//CString str;
	////以下代码演示如何转换为C标准字符串型
	//switch (var.vt)
	//{
	//case VT_EMPTY:{
	//	str = "";
	//	break;
	//}
	//case VT_BSTR:
	//{
	//	str = var.bstrVal;
	//	break;
	//}
	//case VT_I2: //var is short int type 
	//{
	//	str.Format("%d", (int)var.iVal);
	//	break;
	//}
	//case VT_I4: //var is long int type
	//{
	//	str.Format("%d", var.lVal);
	//	break;
	//}
	//case VT_R4: //var is float type
	//{
	//	str.Format("%10.6f", (double)var.fltVal);
	//	break;
	//}
	//case VT_R8: //var is double type
	//{
	//	str.Format("%10.6f", var.dblVal);
	//	break;
	//}
	////case VT_CY: //var is CY type
	////{
	////	str = COleCurrency(var).Format();
	////	break;
	////}
	////case VT_DATE: //var is DATE type
	////{
	////	str = COleDateTime(var).Format();
	////	break;
	////}
	//case VT_BOOL: //var is VARIANT_BOOL
	//{
	//	str = (var.boolVal == 0) ? "FALSE" : "TRUE";
	//	break;
	//}
	//default:
	//{
	//	str.Format("Unk type %d\n", var.vt);
	//	break;
	//}
	//}
	//return str.GetString();
}

v8::Local<v8::Value> getValueFromObject(v8::Local<v8::Object> options, std::string key) {
	v8::Local<v8::String> v8str = Nan::New<v8::String>(key).ToLocalChecked();
	return Nan::Get(options, v8str).ToLocalChecked();
}

v8::Local<v8::String> getStringFromObj(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->ToString();
}




static uv_async_t s_async = { 0 };

#endif //SRC_NODE_OPCDA_H_