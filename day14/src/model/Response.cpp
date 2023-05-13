#include "model/Response.h"

Response::Response(const Request* req) {
  status = 200;
  httpVersion = req->httpVersion;
  head["Server"] = "tinyServer/0.1";
}

Response::~Response() {}

void Response::addHead(const std::string& key, const std::string& value) {
  head.insert_or_assign(key, value);
}
void Response::setStatusCode(uint16_t status) { this->status = status; }

void Response::setBody(const std::string& body) {
  this->body = body;
  head["Content-Length"] = std::to_string(body.size());
}

void Response::setContentType(const std::string& type) {
  head["Content-Type"] = type;
}
