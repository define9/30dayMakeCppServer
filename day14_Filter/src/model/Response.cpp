#include "model/Response.h"

namespace {
const static std::map<uint16_t, const char*> code2Message = {
    {200, "OK"}, {400, "Bad Request"}, {403, "Forbidden"}, {404, "Not Found"}};

const std::string formatDateNow() {
  time_t rawtime;
  struct tm* info;
  char buffer[80];

  time(&rawtime);
  info = localtime(&rawtime);
  strftime(buffer, 80, "%a, %d %b %Y %X %Z", info);
  return std::string(buffer);
}
}  // namespace

Response::Response(const Request* req) {
  _status = 200;
  _httpVersion = req->getVersion();

  // 默认请求头
  _head["Server"] = "tinyServer/0.1";
  _head["Date"] = formatDateNow();
  _head["Content-Length"] = "0";
}

Response::~Response() {}

void Response::addHead(const std::string& key, const std::string& value) {
  _head.insert_or_assign(key, value);
}

void Response::setStatusCode(uint16_t status) { _status = status; }

void Response::setBody(const std::string& body) {
  _body = body;
  _head["Content-Length"] = std::to_string(body.size());
}

void Response::setContentType(const std::string& type) {
  _head["Content-Type"] = type;
}

const std::string Response::serialize2Str() {
  std::stringstream os;
  const std::string message = safeGet(code2Message, _status, "OK");

  os << _httpVersion << " " << _status << " " << message << "\r\n";

  for (auto pair : _head) {
    os << pair.first << ": " << pair.second << "\r\n";
  }

  os << "\r\n" << _body << "\r\n";

  return os.str();
}
