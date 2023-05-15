#include "builder/RequestBuilder.h"

namespace {
enum State {
  ExpectRequestLine = 1,
  ExpectHeaders,
  ExpectBody,
  Done,
};

static const std::map<std::string, Method> method = {{"GET", Method::GET},
                                                     {"POST", Method::POST},
                                                     {"DELETE", Method::DELETE},
                                                     {"HEAD", Method::HEAD},
                                                     {"PUT", Method::PUT}};

static const bool startWith(const std::string& str, const std::string& prefix) {
  return str.rfind(prefix, 0) == 0;
}
static const bool startWith(const std::string& str, char prefix) {
  return str.rfind(prefix, 0) == 0;
}

static const int findI(const std::string& str, char c, size_t i) {
  size_t index = 0;
  while ((index = str.find(c, index)) < str.length()) {
    index++;
    i--;
    if (i == 0) {
      return index - 1;
    }
  }
  return 0;
}

static void url2path(Request* req) {
  if (startWith(req->url, '/')) {  // /api/a
    req->path = req->url;
  } else if (startWith(req->url, "http")) {  // http://local.com/api/a
    int i = findI(req->url, '/', 3);
    req->path = req->url.substr(i);
  } else {  // local.com/api/a
    int i = findI(req->url, '/', 1);
    req->path = req->url.substr(i);
  }
}
}  // namespace

RequestBuilder::RequestBuilder() {}

RequestBuilder::~RequestBuilder() {}

void RequestBuilder::build(Request* req, const Buffer* buf) {
  parseRequest(req, buf);
  inspect(req);
}

void RequestBuilder::inspect(const Request* req) {
  Log::debug("---------- [inspect Request start] ----------");
  Log::debug("method: ", req->method);
  Log::debug("url: ", req->url);
  Log::debug("path: ", req->path);
  Log::debug("httpVersion: ", req->httpVersion);
  for (auto h : req->head) {
    Log::debug("head: ", h.first, ": ", h.second);
  }
  Log::debug("body: ", req->body);
  Log::debug("---------- [inspect Request   end] ----------");
}

bool RequestBuilder::parseRequest(Request* req, const Buffer* buf) {
  Log::debug("parse Http Request [start]");
  State state = State::ExpectRequestLine;
  std::istringstream inStream(buf->c_str());
  std::string line;

  while (getline(inStream, line)) {
    switch (state) {
      case State::ExpectRequestLine: {
        std::istringstream line_stream(line);
        std::string tmp;
        line_stream >> tmp;
        if (tmp.find("HTTP") == std::string::npos) {
          req->method = safeGet(method, tmp, Method::GET);

          line_stream >> tmp;
          req->url = tmp;
          url2path(req);

          line_stream >> tmp;
          req->httpVersion = tmp;
        }
        state = State::ExpectHeaders;
      } break;
      case State::ExpectHeaders: {
        if (line.size() == 1) {  // \r
          state = State::Done;
        } else {
          auto pos = line.find(':');
          if (pos == std::string::npos) {
            continue;
          }
          std::string key(line, 0, pos);
          std::string val(line, pos + 2);
          req->head.insert_or_assign(key, val);
        }
      } break;
      case State::ExpectBody: {
        req->body.append(line).push_back('\n');
        state = State::Done;
      } break;
      case State::Done:
        break;
    }
  }
  Log::debug("parse Http Request [done]");

  return true;
}
