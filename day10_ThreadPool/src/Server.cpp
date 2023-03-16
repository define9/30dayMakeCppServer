#include "Server.h"

Server::Server(const char* ip, uint16_t port) {
  _serverAddr = InetAddress(ip, port);
  init();
}

Server::Server(uint16_t port) {
  _serverAddr = InetAddress(port);
  init();
}

void Server::init() {
  _loop = new EventLoop();
  _acceptor = new Acceptor(&_serverAddr);
  _acceptor->setNewConnection([=](Socket* servSocket, EventLoop* loop){
    newConnection(servSocket, loop);
  });
}

Server::~Server() {
  delete _loop;
  _loop = nullptr;

  delete _acceptor;
  _acceptor = nullptr;
}

void Server::loop() {
  _acceptor->listen(_loop);
  printf("listen ...\n");

  _loop->start();
  _loop->loop();
}

void Server::newConnection(Socket* servSocket, EventLoop* loop) {
  Connection* conn = new Connection(servSocket, loop);
  conn->setDisConnection([=](){
    disConnection(conn);
  });
  _openConnection.insert_or_assign(conn->getCltSocket()->getFd(), conn);
  printf("-----------------------------\n");
  printf("new connection, current connection count: %ld\n", _openConnection.size());
  printf("client IP: %s Port: %d\n", inet_ntoa(conn->getCltAddr()->addr.sin_addr), ntohs(conn->getCltAddr()->addr.sin_port));
  printf("-----------------------------\n");
}

void Server::disConnection(Connection* conn) {
  _openConnection.erase(conn->getCltSocket()->getFd());
  // 我们不需要手动close(fd), 因为他的生命周期由 Socket 管理
  delete conn;
  printf("new connection, current connection count: %ld\n", _openConnection.size());
}
