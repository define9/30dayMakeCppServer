#pragma once
#include <errno.h>

#include <map>
#include <mutex>
#include <sstream>

#include "Acceptor.h"
#include "Connection.h"
#include "Dispatcher.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Log.h"
#include "Socket.h"
#include "model/Request.h"
#include "model/Response.h"

#define READ_BUFFER 1024

class Server {
 private:
  EventLoop* _loop{nullptr};
  Acceptor* _acceptor{nullptr};

  InetAddress _serverAddr;
  std::map<int, Connection*> _openConnection;
  std::mutex _mapLock;

  void init();

 public:
  Server(const char* ip, uint16_t port);
  Server(uint16_t port);
  ~Server();

  void loop();
  void stop();

  void newConnection(Connection* conn);
  void disConnection(Connection* conn);
};
