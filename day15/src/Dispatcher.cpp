#include "Dispatcher.h"

Dispatcher::Dispatcher() {}

Dispatcher::~Dispatcher() {}

void Dispatcher::resolve(const Request* req, Response* resp) {
  auto it = _handles.find(req->path);
  if (it != _handles.end()) {
    // 找到了一个handle
    Log::debug("find a handle, ", req->path);
    it->second(req, resp);
  } else {
    resp->setBody("404");
  }
}

void Dispatcher::addHandle(const std::string& url, HandleFun fun) {
  _handles.insert_or_assign(url, fun);
}
