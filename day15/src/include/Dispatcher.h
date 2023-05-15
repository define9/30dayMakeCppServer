#pragma once

#include <map>
#include <functional>

#include "Log.h"
#include "model/Request.h"
#include "model/Response.h"
#include "util.h"

typedef std::function<void(const Request*, Response*)> HandleFun;

class Dispatcher {
 private:
  std::map<std::string, HandleFun> _handles;
 public:
  Dispatcher();
  ~Dispatcher();

  void addHandle(const std::string& url, HandleFun fun);
  void resolve(const Request*, Response*);
};
