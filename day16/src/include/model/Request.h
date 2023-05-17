#pragma once

#include <map>
#include <string>

#include "util.h"
#include "Log.h"
#include "ReqMethod.h"
using namespace ReqMethod;

class Request {
 public:
  /* 第一行, 也是第一部分 */
  Method method;
  std::string path;
  std::string url;
  std::string httpVersion;

  /* 第二部分, 结束后单独一个\r\n */
  std::map<std::string, std::string> head;

  /* 第三部分 */
  std::string body;

  Request();
  ~Request();
};
