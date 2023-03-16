#pragma once

#include <cstdio>
#include <errno.h>
#include "Channel.h"
#include "InetAddress.h"
#include "EventLoop.h"
#include "Socket.h"

#define READ_BUFFER 1024

class Connection {
 private:
  Socket* _cltSocket{nullptr};
  Channel* _cltChannel{nullptr};
  InetAddress* _cltAddr{nullptr};

  std::function<void()> _cb; // 断开连接的回调函数
 public:
  Connection(Socket* servSocket, EventLoop* loop);
  ~Connection();

  void handle();
  void setDisConnection(std::function<void()> cb);

  InetAddress* getCltAddr();
  Socket* getCltSocket();
};
