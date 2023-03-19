#include "Server.h"

#include <errno.h>
#include <stdio.h>

#include <map>

int main() {
  Server server("0.0.0.0", 8888);
  printf("server init done.\n");
  server.loop();
}
