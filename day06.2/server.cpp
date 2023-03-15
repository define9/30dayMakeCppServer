#include <errno.h>
#include <stdio.h>
#include <map>
#include "src/Server.h"

int main() {
  Server server("0.0.0.0", 8888);
  printf("server init done.\n");
  server.loop();
}
