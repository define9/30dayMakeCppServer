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
  _dispatcher = new Dispatcher();
  _dispatcher->mountDir("./www/");
}

Server::~Server() {
  delete _loop;
  delete _acceptor;
  delete _dispatcher;
}

void Server::loop() {
  _acceptor->listen(_loop);
  Log::info("listen ...");

  _loop->start();
  _loop->loop();
}

void Server::stop() {
  for (auto pair : _openConnection) {
    delete pair.second;
  }

  _loop->stop();
}

void Server::newConnection(Connection* conn) {
  conn->setDisConnection([=]() {
    disConnection(conn);
    _loop->deleteChannel(conn->getChannel());
  });

  conn->setRecvConnection([=](Buffer* inBuf) -> bool {
    const Request* req = new Request(inBuf);
    Response* resp = new Response(req);

    _dispatcher->resolve(req, resp);
    conn->write(resp->serialize2Str(), true);
    delete req;
    delete resp;
    return false;
  });

  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.insert_or_assign(conn->getSocket()->getFd(), conn);
  Log::debug("new connection, current connection count: ",
             _openConnection.size());
  Log::debug("client IP: ", inet_ntoa(conn->getAddr()->addr.sin_addr),
             ", Port: ", ntohs(conn->getAddr()->addr.sin_port));
}

void Server::disConnection(Connection* conn) {
  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.erase(conn->getSocket()->getFd());
  Log::debug("dis connection, current connection count: ",
             _openConnection.size());
}
