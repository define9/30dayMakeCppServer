#include "Dispatcher.h"

Dispatcher::Dispatcher() {
  _defaultFiles.push_back("index.html");
  _errPage[404] = "404.html";
}

Dispatcher::~Dispatcher() {}

void Dispatcher::resolve(const Request* req, Response* resp) {
  Log::info("receive a request, path: ", req->getPath());
  for (auto pair : _handles) {
    if (req->getPath() == pair.first) {
      pair.second(req, resp);
      return;
    }
  }

  // 开始处理静态文件
  const std::string path = _wwwRoot + req->getPath();
  bool useDefault =
      path.at(path.size() - 1) == '/';  // 是否读取默认文件, index.html
  bool found = false;

  if (!useDefault) {
    found = doStaticRequest(path, resp);
  } else {
    for (auto defFile : _defaultFiles) {
      found = doStaticRequest(path + defFile, resp);
      if (found) return;
    }
  }

  if (found) return;

  resp->setStatusCode(404);
  doStaticRequest(_wwwRoot + _errPage[404], resp);
  Log::debug("file not found, path: ", req->getPath());
}

void Dispatcher::mountDir(const std::string& path) { _wwwRoot = path; }

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
  std::string suffix = getSuffix(pathStr);
  resp->setContentType(safeGet(ResponseConstant::suffix2Type, suffix, ""));
  munmap(srcAddr, sbuf.st_size);
  return true;
}

void Dispatcher::addHandle(std::string url, std::function<void(const Request* req, Response* resp)> handle) {
  _handles.insert_or_assign(url, handle);
}
