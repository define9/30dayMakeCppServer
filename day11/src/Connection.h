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
  Socket* _socket{nullptr};
  Channel* _channel{nullptr};
  InetAddress* _addr{nullptr};

  Buffer* _inBuf{nullptr};
  Buffer* _outBuf{nullptr};

  std::function<void()> _delCb;   // 断开连接的回调函数
  std::function<bool(Buffer* buf)> _recvCb;  // 接收消息回调函数

  void readHandle();
  void writeHandle();
 public:
  Connection(int fd, InetAddress* addr);
  /**
   * @brief 使用socket和addr初始化Connection, 并将二者的生命周期交给Connection管理
   * @param socket socket
   * @param addr addr
  */
  Connection(Socket* socket, InetAddress* addr);
  ~Connection();
  void init(int fd);

  void setDisConnection(std::function<void()> cb);
  /// @brief 
  /// @param cb 收到消息自动回调的函数, 返回true会清空缓冲区
  void setRecvConnection(std::function<bool(Buffer* buf)> cb);

  InetAddress* getAddr();
  Socket* getSocket();
  Channel* getChannel();

  /// @brief 向缓存区写入
  /// @param str 
  /// @return 
  int write(std::string str);
};
