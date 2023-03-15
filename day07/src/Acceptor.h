#pragma once
#include <functional>

#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"
#include "Socket.h"

class Acceptor {
 private:
  std::function<void(Socket* servSocket, EventLoop* loop)>
      _newConnection;  // 新建连接回调
  Socket* _serverSocket{nullptr};
  Channel* _serverChannel{nullptr};

 public:
  Acceptor(InetAddress* serverAddr);
  ~Acceptor();

  void setNewConnection(
      std::function<void(Socket* servSocket, EventLoop* loop)> fun);

  /**
   * 接收器 开始监听
   */
  void listen(EventLoop* loop);
};
