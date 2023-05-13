#pragma once

#include <map>
#include <string>

#include "Buffer.h"
#include "util.h"
#include "Log.h"
#include "ReqMethod.h"
using namespace ReqMethod;

class Request {
 private:
  /* 第一行, 也是第一部分 */
  Method _method;
  std::string _path;
  std::string _httpVersion;

  /* 第二部分, 结束后单独一个\r\n */
  std::map<std::string, std::string> _head;

  /* 第三部分 */
  std::string _body;

  bool parseRequest(const Buffer* buf);
  bool parseRequestLine(const char* begin, const char* end);
 public:
  Request(const Buffer* buf);
  ~Request();

  const std::string getPath() const;
  const std::string& getVersion() const;
};
