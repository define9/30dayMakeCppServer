#include "Server.h"

#include <errno.h>
#include <stdio.h>

#include <map>

class TestA : public IController
{
private:
public:
  TestA();
  ~TestA();
  void addRoute(Server* server);
};

TestA::TestA()
{
}

TestA::~TestA()
{
}

void TestA::addRoute(Server* server) {
  server->addHandle("/", [=](const Request* req, Response* resp){
    resp->setBody("我是/");
  });
}

int main() {
  Server server(8080);
  IController* a = new TestA();
  server.registerController(a);
  server.loop();
}
