#include <string.h>
#include <unistd.h>

#include <functional>
#include <iostream>

#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/ThreadPool.h"
#include "src/util.h"

using namespace std;

void oneClient() {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr("127.123.0.1");
  serv_addr.sin_port = htons(8888);

  int a = connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
  connectStatus(a);
  close(sockfd);
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

