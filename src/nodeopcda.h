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
#include <algorithm>


#define ERROR_STRING_SIZE 1024

NAN_METHOD(List);
void EIO_List(uv_work_t* req);
void EIO_AfterList(uv_work_t* req);

NAN_METHOD(Init);
void EIO_Init(uv_work_t* req);
void EIO_AfterInit(uv_work_t* req);

void DataChangeNode(COPCServer *opcServer, COPCGroup *group, std::vector<COPCItem *>itemsCreated, Nan::Callback* dataCallback);
void EIO_Data(uv_work_t* req);
void EIO_AfterData(uv_work_t* req);

struct ListBaton {
	Nan::Callback callback;
	char hostName[1024];
	std::vector<std::string> ServerList;
	char errorString[ERROR_STRING_SIZE];
};

struct InitBatonOptions {	
	v8::Local<v8::String> hostName;
	v8::Local<v8::String> progId;
	Nan::Callback* dataChange;
	Nan::Callback callback;
	v8::Local<v8::String> errorString;
};

struct OptionBaton {
	Nan::Callback callback;
	InitBatonOptions* options;
	char errorString[ERROR_STRING_SIZE];
};

struct DataBaton{
	COPCServer *opcServer;
	COPCGroup *group;
	std::vector<COPCItem *>itemsCreated;
	short cacheData;
	//COPCItem_DataMap dataMap;
	char Str[ERROR_STRING_SIZE];
	bool dataChanged;
	Nan::Callback* dataCallback;
};

#endif //SRC_NODE_OPCDA_H_