#pragma once

#include <sys/mman.h>  // mmap, munmap
#include <sys/stat.h>  // stat

#include <functional>
#include <map>

#include "Log.h"
#include "model/Request.h"
#include "model/Response.h"
#include "util.h"

using namespace ReqMethod;

typedef std::function<void(const Request *, Response *)> HandleFun;

class Dispatcher {
 private:
  std::map<std::string, std::map<Method, HandleFun>> _handles;
  std::map<std::string, std::string> _mounts;

  bool resolveMount(const Request *req, Response *resp);
  /// @brief mmap IO映射出文件
  /// @param path 
  /// @param resp 
  /// @return 
  short redirectFile(const std::string &path, Response *resp);

 public:
  Dispatcher();
  ~Dispatcher();

  void addHandle(const std::string &url, const Method method, HandleFun fun);
  void mount(const std::string &path, const std::string &dir);
  void resolve(const Request *, Response *);
};
