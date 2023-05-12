#include <iostream>

#include "Connection.h"
#include "util.h"

int main() {
  Socket* socket = new Socket();
  InetAddress* addr = new InetAddress("127.0.0.1", 8888);

  delete socket;
  delete addr;
  return 0;
}
