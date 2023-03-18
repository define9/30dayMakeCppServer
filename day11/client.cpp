#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "src/Connection.h"
#include "src/util.h"

#define BUFFER_SIZE 512

int main() {
  Socket* socket = new Socket();
  InetAddress* addr = new InetAddress("127.0.0.1", 8888);
  socket->connect(addr);
  int fd = socket->getFd();
  Connection* conn = new Connection(socket, addr);

  while (true) {
    int write_bytes = conn->write("client haha!");
    if (write_bytes == -1) {
      printf("socket already disconnected, can't write any more!\n");
      break;
    }
    sleep(1);
  }
  delete conn;
  return 0;
}
