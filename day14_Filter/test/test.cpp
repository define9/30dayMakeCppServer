#include <string.h>
#include <unistd.h>

#include <functional>
#include <iostream>

#include "Buffer.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "util.h"
#include "model/Request.h"

using namespace std;

void oneClient() {
  Socket* socket = new Socket();
  InetAddress* addr = new InetAddress("127.0.0.1", 8080);
  socket->connect(addr);
  Connection* conn = new Connection(socket, addr);
  conn->setRecvConnection([=](Buffer* buf) -> bool {
    std::cout << buf->c_str() << std::endl;
    return false;
  });

  conn->write("12345", true);
  conn->read(true);
  delete conn;
}

int main(int argc, char *argv[]) {
  int threads = 100;
  int msgs = 100;
  int wait = 0;
  int o;
  const char *optstring = "t:m:w:";
  while ((o = getopt(argc, argv, optstring)) != -1) {
    switch (o) {
      case 't':
        threads = stoi(optarg);
        break;
      case 'm':
        msgs = stoi(optarg);
        break;
      case 'w':
        wait = stoi(optarg);
        break;
      case '?':
        printf("error optopt: %c\n", optopt);
        printf("error opterr: %d\n", opterr);
        break;
    }
  }

  ThreadPool *pool = new ThreadPool();
  pool->start(threads);
  for (int i = 0; i < threads; ++i) {
    pool->addTask([=]() {
      printf("thread %d start\n", i);
      oneClient();
    });
  }
  delete pool;
  return 0;
}
