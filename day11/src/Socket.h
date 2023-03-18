#pragma once
#include <arpa/inet.h>
#include <fcntl.h>       //设置非阻塞
#include <sys/socket.h>  //创建socket
#include <unistd.h>

#include "InetAddress.h"
#include "util.h"

class Socket {
 private:
  int _fd;
  volatile bool _nonBlock;

 public:
  Socket();

  /**
   * @brief 使用此构造方法, fd将由 此类管理
  */
  Socket(int);
  ~Socket();

  void bind(InetAddress*);
  void listen();
  void setnonblocking();

  int accept(InetAddress*);
  void connect(InetAddress*);

  int getFd();
  bool isNonBlock();
};
