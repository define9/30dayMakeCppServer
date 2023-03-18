#include "Socket.h"

Socket::Socket() {
  _fd = socket(AF_INET, SOCK_STREAM, 0);
  errif(_fd < 0, "socket create error");
  _nonBlock = false;
}

Socket::Socket(int fd) : _fd(fd) {
  errif(_fd < 0, "socket create error");
  _nonBlock = false;
}

Socket::~Socket() {
  if (_fd >= 0) {
    close(_fd);
    _fd = -1;
  }
}

void Socket::bind(InetAddress* addr) {
  errif(::bind(_fd, (sockaddr*)&addr->addr, addr->addr_len) < 0,
        "socket bind error");
}

void Socket::listen() { errif(::listen(_fd, SOMAXCONN) < 0, "listen error"); }

void Socket::setnonblocking() {
  fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL) | O_NONBLOCK);
  _nonBlock = true;
}

int Socket::accept(InetAddress* addr) {
  int fd = guard(::accept(_fd, (sockaddr*)&addr->addr, &addr->addr_len),
                 "accept error");
  return fd;
}

void Socket::connect(InetAddress* addr) {
  struct sockaddr_in serv_addr = addr->addr;
  ::connect(_fd, (sockaddr*)&serv_addr, sizeof(serv_addr));
}

int Socket::getFd() { return _fd; }

bool Socket::isNonBlock() { return _nonBlock; }
