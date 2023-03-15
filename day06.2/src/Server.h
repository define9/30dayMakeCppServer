#pragma once
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include <errno.h>

#define READ_BUFFER 1024

class Server {
 private:
  EventLoop* _loop;
  Socket* _serverSocket;

  InetAddress _serverAddr;

  void init();
 public:
  Server(const char* ip, uint16_t port);
  Server(InetAddress serverAddr);
  ~Server();

  void loop();

  void newConnection(Socket* servSocket, EventLoop* loop);
  void handleReadEvent(Socket* socket, Channel* channel);
};
