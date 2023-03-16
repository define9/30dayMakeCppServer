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

 public:
  Socket();
  Socket(int);
  ~Socket();

  void bind(InetAddress*);
  void listen();
  void setnonblocking();

  int accept(InetAddress*);

  int getFd();
};
