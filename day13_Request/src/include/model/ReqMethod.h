#pragma once
#include <string>

namespace ReqMethod {
enum Method {
  GET,
  POST,
  HEAD,
  PUT,
  DELETE,
};

class MethodConst {
 public:
  MethodConst() {}
  ~MethodConst() {}
};

}  // namespace ReqMethod
