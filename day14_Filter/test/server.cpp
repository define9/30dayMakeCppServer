#include "Server.h"

#include <errno.h>
#include <stdio.h>

#include <map>

int main() {
  Server server(8080);
  printf("server init done.\n");
  server.loop();
}
