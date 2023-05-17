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
    // 没有找到path的handle, 开始计算静态文件
    bool completed = resolveMount(req, resp);

    if (completed) {
      return;
    }

    resp->status = 404;
    resp->setBody("404");
  }
}

void Dispatcher::addHandle(const std::string& url, const Method method,
                           HandleFun fun) {
  _handles[url].insert_or_assign(method, fun);
}

void Dispatcher::mount(const std::string& path, const std::string& dir) {
  _mounts[path] = dir;
}

bool Dispatcher::resolveMount(const Request* req, Response* resp) {
  if (req->method != Method::GET) {
    return false;
  }

  for (auto it : _mounts) {
    if (startWith(req->path, it.first)) {
      // req.path: /s/a.txt
      // it.first: /s
      // it.second: ./www
      const std::string realPath =
          it.second + req->path.substr(it.first.length());
      // realPath: ./www/a.txt
      Log::warn("realPath: ", realPath);
      short code = redirectFile(realPath, resp);
      Log::warn("code: ", code);
      if (code == 0) {
        return true;
      }
    }
  }
  return false;
}

short Dispatcher::redirectFile(const std::string& path, Response* resp) {
  struct stat sbuf;
  const char* p = path.data();

  // 文件找不到错误
  if (::stat(p, &sbuf) < 0) {
    return 1;
  }

  // 权限错误
  if (!(S_ISREG(sbuf.st_mode) || !(S_IRUSR & sbuf.st_mode))) {
    // resp->setStatusCode(404);
    // resp->setBody("can't read the file");
    return 2;
  }

  // 报文体
  int fd = ::open(p, O_RDONLY, 0);
  // 存储映射IO
  void* mmapRet = ::mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  ::close(fd);

  if (mmapRet == (void*)-1) {
    munmap(mmapRet, sbuf.st_size);
    // resp->setStatusCode(404);
    // resp->setBody("can't find the file");
    return 3;
  }

  char* srcAddr = static_cast<char*>(mmapRet);
  resp->setBody(std::string(srcAddr, sbuf.st_size));
  munmap(srcAddr, sbuf.st_size);
  return 0;
}