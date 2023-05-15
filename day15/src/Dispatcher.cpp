#include "Dispatcher.h"

Dispatcher::Dispatcher() {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::resolve(const Request* req, Response* resp) {
  resp->setBody("hi");
}
