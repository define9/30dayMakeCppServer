#include "Dispatcher.h"

Dispatcher::Dispatcher() { _defaultFiles.push_back("index.html"); }

Dispatcher::~Dispatcher() {}

void parseResponseType(const std::string& suffix, Response* resp) {
  const std::string& type = safeGet(ResponseConstant::suffix2Type, suffix, "");
  resp->setContentType(type);
}

void Dispatcher::resolve(const Request* req, Response* resp) {
  Log::info("receive a request, path: ", req->getPath());
  for (auto pair : _handles) {
    if (req->getPath() == pair.first) {
      pair.second(req, resp);
      return;
    }
  }

  // 开始处理静态文件
  const std::string path = req->getPath();
  bool useDefault =
      path.at(path.size() - 1) == '/';  // 是否读取默认文件, index.html
  bool found = false;

  for (auto dir : _dir) {
    if (!useDefault) {
      found = doStaticRequest(dir + path, resp);
    } else {
      for (auto defFile : _defaultFiles) {
        found = doStaticRequest(dir + path + defFile, resp);
        if (found) break;
      }
    }
  }

  if (found) return;

  resp->setStatusCode(404);
  resp->setBody("");
  Log::warn("file not found, path: ", req->getPath());
}

void Dispatcher::mountDir(const std::string& path) {
  if (_dir.size() > 0) {
    Log::warn("folder is mounted");
    return;
  }

  _dir.push_back(_rootPath + path);
}

bool Dispatcher::doStaticRequest(const std::string& pathStr, Response* resp) {
  struct stat sbuf;
  const char* path = pathStr.data();

  // 文件找不到错误
  if (::stat(path, &sbuf) < 0) {
    return false;
  }

  // 权限错误
  if (!(S_ISREG(sbuf.st_mode) || !(S_IRUSR & sbuf.st_mode))) {
    // resp->setStatusCode(404);
    // resp->setBody("can't read the file");
    return false;
  }

  // 报文体
  int fd = ::open(path, O_RDONLY, 0);
  // 存储映射IO
  void* mmapRet = ::mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  ::close(fd);

  if (mmapRet == (void*)-1) {
    munmap(mmapRet, sbuf.st_size);
    // resp->setStatusCode(404);
    // resp->setBody("can't find the file");
    return false;
  }

  char* srcAddr = static_cast<char*>(mmapRet);
  resp->setBody(std::string(srcAddr, sbuf.st_size));
  munmap(srcAddr, sbuf.st_size);
  return true;
}
