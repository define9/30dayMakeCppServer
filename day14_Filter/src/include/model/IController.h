#pragma once

#include <map>
#include <string>
#include <vector>

#include "Log.h"
#include "Server.h"

class Server;

class IController {
 private:
 public:
  IController(){};
  ~IController(){};

  virtual void addRoute(Server* server) = 0;
};
