#pragma once

#include <sys/mman.h>  // mmap, munmap
#include <sys/stat.h>  // stat

#include <fstream>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "model/Request.h"
#include "model/Response.h"

class Dispatcher {
 private:
  std::string _rootPath;
  std::map<std::string, std::function<void(const Request* req, Response* resp)>>
      _handles;
  std::string _wwwRoot; // 挂载的目录

  std::vector<std::string> _defaultFiles;
  std::map<uint16_t, std::string> _errPage;

  bool doStaticRequest(const std::string& pathStr, Response* resp);

 public:
  Dispatcher();
  ~Dispatcher();

  void resolve(const Request* req, Response* resp);

  /// @brief 挂载一个静态目录
  /// @param path
  void mountDir(const std::string& path);

  void addHandle(std::string url, std::function<void(const Request* req, Response* resp)> handle);
};
