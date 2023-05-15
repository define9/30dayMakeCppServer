#pragma once

#include "Log.h"
#include "util.h"
#include "model/Request.h"
#include "model/Response.h"

class Dispatcher
{
private:
public:
  Dispatcher();
  ~Dispatcher();

  void resolve(const Request*, Response*);
};
