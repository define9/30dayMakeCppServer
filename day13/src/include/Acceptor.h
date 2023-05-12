#pragma once
#include <functional>

#include "Channel.h"
#include "Connection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Timer.h"

class Acceptor {
 private:
  std::function<void(Connection* conn)> _conn;
  Socket* _serverSocket{nullptr};                    // 服务端的socket
  Channel* _serverChannel{nullptr};                  // 服务端的channel
  Timer* _timer;

 public:
  Acceptor(InetAddress* serverAddr);
  ~Acceptor();

  void setConn(std::function<void(Connection*)> fun);

  /**
   * 接收器 开始监听
   */
  void listen(EventLoop* loop);
};
