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
	uint64_t Rate_Ms;
	double DeadBand;
	std::vector<std::string>ItemNames;
	Nan::Callback* DataCallback;
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

std::string GetStr(VARIANT var){
	CString str;
	if (var.vt == VT_BOOL)
		str = (var.boolVal == 0) ? "FALSE" : "TRUE";
	else
	{
		_variant_t temp(var);
		temp.ChangeType(VT_BSTR);
		str = temp.bstrVal;
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

bool getBoolFromObject(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->ToBoolean()->BooleanValue();
}

int getIntFromObject(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->Int32Value();
}

uint64_t getLongFromObject(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->IntegerValue();
}

double getDoubleFromObject(v8::Local<v8::Object> options, std::string key) {
	return getValueFromObject(options, key)->NumberValue();
}

static uv_async_t s_async = { 0 };

#endif //SRC_NODE_OPCDA_H_