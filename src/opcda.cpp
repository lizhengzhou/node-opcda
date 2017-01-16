#include"nodeopcda.h"


using namespace v8;

void EIO_List(uv_work_t* req) {
	ListBaton* data = static_cast<ListBaton*>(req->data);

	COPCClient::init();
	std::string hostName = data->hostName;
	COPCHost *host = COPCClient::makeHost(hostName);
	//List servers
	std::vector<std::string> localServerList = data->ServerList;
	host->getListOfDAServers(IID_CATID_OPCDAServer20, data->ServerList);
}

void EIO_AfterList(uv_work_t* req) {
	Nan::HandleScope scope;

	ListBaton* data = static_cast<ListBaton*>(req->data);

	v8::Local<v8::Value> argv[2];
	if (data->errorString[0]) {
		argv[0] = v8::Exception::Error(Nan::New<v8::String>(data->errorString).ToLocalChecked());
		argv[1] = Nan::Undefined();
	}
	else {
		v8::Local<v8::Array> results = Nan::New<v8::Array>();
		int i = 0;
		for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it, i++) {
			Nan::Set(results, i, Nan::New<v8::String>(*it).ToLocalChecked());
		}
		argv[0] = Nan::Null();
		argv[1] = results;
	}
	data->callback.Call(2, argv);

	//for (std::vector<std::string>::iterator it = data->ServerList.begin(); it != data->ServerList.end(); ++it) {
	//	delete *it;
	//}
	delete data;
	delete req;
}



NAN_METHOD(List) {
	if (!info[0]->IsString()) {
		Nan::ThrowTypeError("First argument must be a string");
		return;
	}
	v8::String::Utf8Value host(info[0]->ToString());
	// callback
	if (!info[1]->IsFunction()) {
		Nan::ThrowTypeError("Second argument must be a function");
		return;
	}

	ListBaton* baton = new ListBaton();
	strcpy(baton->hostName, *host);
	strcpy(baton->errorString, "");
	baton->callback.Reset(info[1].As<v8::Function>());

	uv_work_t* req = new uv_work_t();
	req->data = baton;
	uv_queue_work(uv_default_loop(), req, EIO_List, (uv_after_work_cb)EIO_AfterList);

	return;
}


extern "C" {
	void Init(v8::Handle<v8::Object> target) {
		Nan::HandleScope scope;
		Nan::SetMethod(target, "list", List);
		//Nan::SetMethod(target, "open", Open);
		//Nan::SetMethod(target, "update", Update);
		//Nan::SetMethod(target, "write", Write);
		//Nan::SetMethod(target, "close", Close);
		//Nan::SetMethod(target, "list", List);
		//Nan::SetMethod(target, "flush", Flush);
		//Nan::SetMethod(target, "drain", Drain);
	}
}

NODE_MODULE(opcda, Init)



