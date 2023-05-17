#pragma once

#include <map>

#include "Buffer.h"
#include "model/Request.h"
#include "util.h"
#include <sstream>

class RequestBuilder {
 private:
  /// @brief 解析方法,路径,协议版本,head,body
  /// @param req 
  /// @param buf 
  /// @return 
  static bool parseRequest(Request* req, const Buffer* buf);

 public:
  RequestBuilder();
  ~RequestBuilder();

  static void build(Request* req, const Buffer* buf);
  
  static void inspect(const Request* req);
};
