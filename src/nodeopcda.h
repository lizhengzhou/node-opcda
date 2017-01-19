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
#include <map>
#include<iostream>
#include <exception>



#define ERROR_STRING_SIZE 1024


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
void DataChangeNode(WatchBaton* baton);
void EIO_WatchData(uv_work_t* req);
void EIO_AfterWatchData(uv_work_t* req);

#endif //SRC_NODE_OPCDA_H_