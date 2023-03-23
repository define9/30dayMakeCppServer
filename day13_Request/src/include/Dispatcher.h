#pragma once

#include <sys/mman.h>  // mmap, munmap
#include <sys/stat.h>  // stat

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "model/Request.h"
#include "model/Response.h"

class Dispatcher {
 private:
  std::string _rootPath;
  std::map<std::string, std::function<void(Request* req, Response* resp)>>
      _handles;
  std::vector<std::string> _dir;

  bool doStaticRequest(Request* req, Response* resp);

 public:
  Dispatcher();
  ~Dispatcher();

  void resolve(Request* req, Response* resp);

  /// @brief 挂载一个静态目录
  /// @param path
  void mountDir(const std::string& path);
};
