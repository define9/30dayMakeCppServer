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

namespace ResponseConstant {
const static std::map<std::string, const char*> suffix2Type = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css"}};
}  // namespace ResponseConstant

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
  void setContentType(const std::string& type);

  const std::string serialize2Str();
};
