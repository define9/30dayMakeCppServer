#include "model/Request.h"

namespace {
enum State {
  ExpectRequestLine = 1,
  ExpectHeaders,
  ExpectBody,
  Done,
};

const static std::map<std::string, Method> method = {{"GET", Method::GET},
                                                     {"POST", Method::POST},
                                                     {"DELETE", Method::DELETE},
                                                     {"HEAD", Method::HEAD},
                                                     {"PUT", Method::PUT}};

const char* findCRLF(const std::string& buf, std::size_t startIndex = 0) {
  const char CRLF[] = "\r\n";
  auto i = buf.find(CRLF, startIndex);
  if (i == std::string::npos) {
    return nullptr;
  }
  return buf.c_str() + i;
}

}  // namespace

Request::Request(const Buffer* buf) { parseRequest(buf); }

Request::~Request() {}

bool Request::parseRequest(const Buffer* buf) {
  Log::debug("parse Http Request [start]");
  State state = State::ExpectRequestLine;
  bool hasMore = true;

  while (hasMore) {
    switch (state) {
      case State::ExpectRequestLine: {
        const char* crlf = findCRLF(buf->c_str());
        if (crlf != nullptr) {
          parseRequestLine(buf->c_str(), crlf);
          state = State::ExpectHeaders;
        }
      } break;
      case State::ExpectHeaders:
        state = State::Done;
        break;
      case State::ExpectBody:
        state = State::Done;
        break;
      case State::Done:
        hasMore = false;
        break;
    }
  }
  Log::debug("parse Http Request [done]");
  Log::debug("http method: ", _method);
  Log::debug("http path: ", _path);
  Log::debug("http version: ", _httpVersion);

  return true;
}

const std::string Request::getPath() const { return _path; }

bool Request::parseRequestLine(const char* begin, const char* end) {
  auto vs = stringSplit(std::string(begin, end), ' ');

  _method = safeGet(method, vs[0], Method::GET);
  _path = vs[1];
  _httpVersion = vs[2];

  return true;
}

const std::string& Request::getVersion() const { return _httpVersion; }