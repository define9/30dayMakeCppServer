#pragma once
#include "Log.h"
#include "util.h"
#include "model/Response.h"


class ResponseBuilder
{
private:
public:
  ResponseBuilder();
  ~ResponseBuilder();

  static const std::string serialize(Response* resp);
};

