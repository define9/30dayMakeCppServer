#include "Dispatcher.h"

Dispatcher::Dispatcher() {}

Dispatcher::~Dispatcher() {}

void Dispatcher::resolve(Request* req, Response* resp) {
  for (auto pair : _handles) {
    if (req->getPath() == pair.first) {
      pair.second(req, resp);
      return;
    }
  }

  for (auto dir : _dir) {
    if (req->getPath() == dir) {
    }
  }
}

void Dispatcher::mountDir(const std::string& path) {
  _dir.push_back(_rootPath + path);
}

bool Dispatcher::doStaticRequest(Request* req, Response* resp) {
  struct stat sbuf;
  const char* path = req->getPath().data();

  // 文件找不到错误
  if (::stat(path, &sbuf) < 0) {
    return false;
  }

  // 权限错误
  if (!(S_ISREG(sbuf.st_mode) || !(S_IRUSR & sbuf.st_mode))) {
    resp->setStatusCode(404);
    resp->setBody("can't read the file");
    return true;
  }

  // 报文体
  int fd = ::open(req->getPath().data(), O_RDONLY, 0);
  // 存储映射IO
  void* mmapRet = ::mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  ::close(fd);

  if (mmapRet == (void*)-1) {
    munmap(mmapRet, sbuf.st_size);
    resp->setStatusCode(404);
    resp->setBody("can't find the file");
    return false;
  }

  char* srcAddr = static_cast<char*>(mmapRet);
  resp->setBody(std::string(srcAddr, sbuf.st_size));
  munmap(srcAddr, sbuf.st_size);
  return true;
}