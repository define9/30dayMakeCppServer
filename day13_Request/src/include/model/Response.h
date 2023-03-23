#pragma once

#include <time.h>

#include <map>
#include <sstream>
#include <string>

#include "Log.h"
#include "ReqMethod.h"
#include "Request.h"
#include "util.h"

using namespace ReqMethod;

class Response {
 private:
  /* 第一行, 也是第一部分 */
  std::string _httpVersion;

  /// @brief 状态码
  uint16_t _status;

  /* 第二部分 */
  std::map<std::string, std::string> _head;

  /* 第三部分 */
  std::string _body;

 public:
  Response(const Request* req);
  ~Response();

  void addHead(const std::string& key, const std::string& value);
  void setStatusCode(uint16_t status);

  void setBody(const std::string& body);

  const std::string serialize2Str();
};
