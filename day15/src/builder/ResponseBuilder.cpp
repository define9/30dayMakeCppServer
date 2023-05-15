#include "builder/ResponseBuilder.h"

namespace {
const static std::map<uint16_t, const char*> code2Message = {
    {200, "OK"}, {400, "Bad Request"}, {403, "Forbidden"}, {404, "Not Found"}};
}  // namespace

ResponseBuilder::ResponseBuilder(/* args */) {}

ResponseBuilder::~ResponseBuilder() {}

const std::string ResponseBuilder::serialize(Response* resp) {
  std::stringstream os;
  const std::string message = safeGet(code2Message, resp->status, "OK");

  os << resp->httpVersion << " " << resp->status << " " << message << "\r\n";

  for (auto pair : resp->head) {
    os << pair.first << ": " << pair.second << "\r\n";
  }

  os << "\r\n" << resp->body << "\r\n";

  Log::debug("response: ", os.str());

  return os.str();
}