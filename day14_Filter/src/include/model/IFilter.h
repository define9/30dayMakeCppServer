#pragma once

#include <string>

#include "Request.h"
#include "Response.h"

class IFilter {
 private:
 public:
  IFilter(){};
  ~IFilter(){};

  /// @brief 调用, 这里与拦截器差不多
  /// @param req 请求
  /// @param resp 响应
  /// @return 是否继续调用
  virtual bool invoke(Request* req, Response* resp) = 0;
};
