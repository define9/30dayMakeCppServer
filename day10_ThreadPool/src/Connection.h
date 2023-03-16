#pragma once

#include <errno.h>

#include <cstdio>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"

#define PRE_READ_SIZE 1024

class Connection {
 private:
  Socket* _cltSocket{nullptr};
  Channel* _cltChannel{nullptr};
  InetAddress* _cltAddr{nullptr};

  Buffer* _inBuf{nullptr};
  Buffer* _outBuf{nullptr};

  std::function<void()> _cb;  // 断开连接的回调函数
 public:
  Connection(Socket* servSocket, EventLoop* loop);
  ~Connection();

  void handle();
  void setDisConnection(std::function<void()> cb);

  InetAddress* getCltAddr();
  Socket* getCltSocket();
};
