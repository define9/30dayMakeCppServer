#include "builder/RequestBuilder.h"

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

const int findByCArray(const std::string& buf, const char array[],
                       std::size_t startIndex = 0) {
  auto i = buf.find(array, startIndex);
  if (i == std::string::npos) {
    return -1;
  }
  return i;
}

}  // namespace

RequestBuilder::RequestBuilder() {}

RequestBuilder::~RequestBuilder() {}

Request* RequestBuilder::build(const Buffer* buf) {
  Request* req = new Request();
  parseRequest(req, buf);
  return req;
}

bool RequestBuilder::parseRequestLine(Request* req, const char* begin,
                                      const size_t len) {
  std::string str = std::string(begin, begin + len);
  auto vs = stringSplit(str, ' ');

  req->method = safeGet(method, vs[0], Method::GET);
  req->path = vs[1];
  req->httpVersion = vs[2];

  return true;
}

bool RequestBuilder::parseRequest(Request* req, const Buffer* buf) {
  Log::debug("parse Http Request [start]");
  State state = State::ExpectRequestLine;
  bool hasMore = true;

  while (hasMore) {
    switch (state) {
      case State::ExpectRequestLine: {
        const int crlf = findByCArray(buf->c_str(), "\r\n");
        if (crlf != -1) {
          parseRequestLine(req, buf->c_str(), crlf);
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

  return true;
}
