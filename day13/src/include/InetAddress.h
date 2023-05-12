#pragma once
#include <arpa/inet.h>
#include <cstring>

class InetAddress {
 public:
  struct sockaddr_in addr;
  socklen_t addr_len;
  InetAddress();
  InetAddress(uint16_t port);
  InetAddress(const char* ip, uint16_t port);
  ~InetAddress();
};