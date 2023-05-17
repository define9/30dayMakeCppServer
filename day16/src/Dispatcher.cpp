#include "Dispatcher.h"

Dispatcher::Dispatcher() {}

Dispatcher::~Dispatcher() {}

void Dispatcher::resolve(const Request* req, Response* resp) {
  auto it = _handles.find(req->path);
  if (it != _handles.end()) {
    // 找到了一个 path
    auto handle = it->second.find(req->method);
    if (handle != it->second.end()) {
      handle->second(req, resp);
    } else {
      // method not support
      resp->status = 405;
      resp->setBody("not support");
    }
  } else {
    resp->status = 404;
    resp->setBody("404");
  }
}

void Dispatcher::addHandle(const std::string& url, const Method method,
                           HandleFun fun) {
  _handles[url].insert_or_assign(method, fun);
}
