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
  _acceptor->setConn([=](Connection* conn) { newConnection(conn); });
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

void Server::newConnection(Connection* conn) {
  conn->setDisConnection([=]() {
    _loop->deleteChannel(conn->getChannel());
    disConnection(conn);
  });

  conn->setRecvConnection([=](Buffer* inBuf) -> bool {
    std::stringstream fmt;
    fmt << "you say: " << inBuf->c_str() << ", your port: " << ntohs(conn->getAddr()->addr.sin_port) << std::endl;
    conn->write(fmt.str());
    return false;
  });

  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.insert_or_assign(conn->getSocket()->getFd(), conn);
  printf("new connection, current connection count: %ld\n",
         _openConnection.size());
  printf("client IP: %s Port: %d\n", inet_ntoa(conn->getAddr()->addr.sin_addr),
         ntohs(conn->getAddr()->addr.sin_port));
}

void Server::disConnection(Connection* conn) {
  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.erase(conn->getSocket()->getFd());
  printf("dis connection, current connection count: %ld\n",
         _openConnection.size());
  printf("client IP: %s Port: %d\n", inet_ntoa(conn->getAddr()->addr.sin_addr),
         ntohs(conn->getAddr()->addr.sin_port));
}
