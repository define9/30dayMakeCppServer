#include <iostream>

#include "Connection.h"
#include "util.h"

int main() {
  Socket* socket = new Socket();
  InetAddress* addr = new InetAddress("127.0.0.1", 8888);
  socket->connect(addr);
  int fd = socket->getFd();
  Connection* conn = new Connection(socket, addr);
  conn->setRecvConnection([=](Buffer* buf) -> bool {
    std::cout << buf->c_str() << std::endl;
    return false;
  });

  while (true) {
    std::string str;
    std::cin >> str;
    conn->write(str, true);
    conn->read(true);
  }
  delete conn;
  return 0;
}
