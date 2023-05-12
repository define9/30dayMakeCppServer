#pragma once

#include <errno.h>

#include <cstdio>

#include "Buffer.h"
#include "Channel.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Log.h"
#include "Socket.h"

#define PRE_READ_SIZE 1024

class Connection {
 private:
  Socket* _socket{nullptr};
  Channel* _channel{nullptr};
  InetAddress* _addr{nullptr};

  Buffer* _inBuf{nullptr};
  Buffer* _outBuf{nullptr};

  std::function<void()> _cb;  // 断开连接的回调函数
  std::function<void(Buffer* in, Buffer* out)> _handle;  // 对于输入，处理成输出
 public:
  Connection(int fd, InetAddress* addr);
  ~Connection();

  void handle();
  void setHandle(std::function<void(Buffer* in, Buffer* out)> handle);
  void setDisConnection(std::function<void()> cb);

  InetAddress* getAddr();
  Socket* getSocket();
  Channel* getChannel();
};
