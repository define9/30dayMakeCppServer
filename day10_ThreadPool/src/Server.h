#pragma once
#include <errno.h>
#include <map>

#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Connection.h"
#include "Socket.h"

#define READ_BUFFER 1024

class Server {
 private:
  EventLoop* _loop{nullptr};
  Acceptor* _acceptor{nullptr};

  InetAddress _serverAddr;
  std::map<int, Connection*> _openConnection;

  void init();

 public:
  Server(const char* ip, uint16_t port);
  Server(uint16_t port);
  ~Server();

  void loop();

  void newConnection(Socket* servSocket, EventLoop* loop);
  void disConnection(Connection* conn);
};
