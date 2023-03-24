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

  bool readHandle();
  void writeHandle();

  bool readNonBlock();
  bool readBlock();

  void writeNonBlock();
  void writeBlock();
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
  /// @param cb 收到消息自动回调的函数, 返回 true 会保留缓存区
  void setRecvConnection(std::function<bool(Buffer* buf)> cb);

  InetAddress* getAddr();
  Socket* getSocket();
  Channel* getChannel();

  /// @brief 写入
  /// @param str 
  /// @param force 为 true 则强行写入, 不会等待 epoll 调用
  /// @return 
  void write(std::string str, bool force=false);
  std::string read(bool force=false);
};
