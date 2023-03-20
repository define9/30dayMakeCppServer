#include "model/Request.h"

namespace {
enum State {
  ExpectRequestLine,
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

bool parseRequestLine(const char* begin, const char* end) {
  bool succ = false;

  return succ;
}
}  // namespace

Request::Request(const Buffer& buf) { parseRequest(buf); }

Request::~Request() {}

bool Request::parseRequest(const Buffer& buf) {
  State state = State::ExpectRequestLine;
  const char* p = buf.c_str();
  bool hasMore = true;

  while (hasMore) {
    if (state == State::ExpectRequestLine) {
    }
    switch (state) {
      case State::ExpectRequestLine:
        const char* crlf = findCRLF(buf.str());
        if (crlf != nullptr) {
          parseRequestLine(buf.c_str(), crlf);
          state = State::ExpectHeaders;
        }
        break;
      case State::ExpectHeaders:
        break;
      case State::ExpectBody:
        break;
      case Done:
      default:
        break;
    }
  }

  return true;
}

const std::string Request::getPath() { return _path; }
