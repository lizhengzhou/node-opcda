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



#define ERROR_STRING_SIZE 1024

NAN_METHOD(List);
void EIO_List(uv_work_t* req);
void EIO_AfterList(uv_work_t* req);




struct ListBaton {
	Nan::Callback callback;
	char hostName[1024];
	std::vector<std::string> ServerList;
	char errorString[ERROR_STRING_SIZE];
};



#endif //SRC_NODE_OPCDA_H_