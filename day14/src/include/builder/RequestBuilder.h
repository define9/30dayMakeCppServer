#pragma once

#include <map>

#include "Buffer.h"
#include "model/Request.h"
#include "util.h"

class RequestBuilder {
 private:
  static bool parseRequest(Request* req, const Buffer* buf);
  static bool parseRequestLine(Request* req, const char* begin, const size_t len);

 public:
  RequestBuilder();
  ~RequestBuilder();

  static Request* build(const Buffer* buf);
};
