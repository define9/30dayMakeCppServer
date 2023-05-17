#pragma once

#include <functional>
#include <map>

#include "Log.h"
#include "model/Request.h"
#include "model/Response.h"
#include "util.h"

using namespace ReqMethod;

typedef std::function<void(const Request*, Response*)> HandleFun;

class Dispatcher {
 private:
  std::map<std::string, std::map<Method, HandleFun>> _handles;

 public:
  Dispatcher();
  ~Dispatcher();

  void addHandle(const std::string& url, const Method method, HandleFun fun);
  void resolve(const Request*, Response*);
};
