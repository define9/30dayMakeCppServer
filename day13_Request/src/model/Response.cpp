#include "model/Response.h"

namespace {
const static std::map<uint16_t, const char*> code2Message = {
    {200, "OK"}, {400, "Bad Request"}, {403, "Forbidden"}, {404, "Not Found"}};

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
}  // namespace

Response::Response(const Request* req) {
  _status = 200;
  _httpVersion = req->getVersion();
}

Response::~Response() {}

void Response::addHead(const std::string& key, const std::string& value) {
  _head.insert_or_assign(key, value);
}
void Response::setStatusCode(uint16_t status) { _status = status; }

void Response::setBody(const std::string& body) {
  _body = body;
  _head["Content-Length"] = std::to_string(body.size());
  _head["Content-Type"] = "text/plain;charset=UTF-8";
  _head["Server"] = "tinyServer/0.1";
}

const std::string Response::serialize2Str() {
  std::stringstream os;
  const std::string message = safeGet(code2Message, _status, "OK");

  os << _httpVersion << " " << _status << " " << message << "\r\n";

  for (auto pair : _head) {
    os << pair.first << ": " << pair.second << "\r\n";
  }

  os << "\r\n" << _body << "\r\n";

  Log::debug("response: ", os.str());

  return os.str();
}