#include "Server.h"

#include <errno.h>
#include <stdio.h>

#include <map>

int main() {
  Server *server = new Server("0.0.0.0", 8888);
  printf("server init done.\n");

  server->post("/echo", [=](const Request *req, Response *resp) {
    resp->setBody("hi");
  });

  server->loop();
}
