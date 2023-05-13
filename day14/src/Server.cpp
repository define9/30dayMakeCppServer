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
  _timer = new Timer();

  _loop->updateChannel(_timer->getChannel());
}

Server::~Server() {
  for (auto it : _openConnection) {
    Log::debug("dis conn: ", it.first);
    it.second->disConnect();
  }
  _openConnection.clear();
  delete _loop;
  delete _acceptor;
  delete _timer;
}

void Server::loop() {
  _acceptor->listen(_loop);
  Log::debug("listen ...");

  _loop->start();
  _loop->loop();
}

/// @brief Acceptor回调，当有连接进入调用
/// @param conn Acceptor 负责new的连接对象
void Server::newConnection(Connection* conn) {
  conn->setDisConnection([=]() {
    _loop->deleteChannel(conn->getChannel());
    disConnection(conn);
  });
  conn->setHandle([=](Buffer* in, Buffer* out) {
    auto job = _timer->addTask(5, [=]() {
      Log::debug("timeout...");
      conn->disConnect();
    });

    Request req(in);
    conn->setKeepAlive(false);
    out->append("you say ")
        ->append(in->c_str())
        ->append(", you port: ")
        ->append(std::to_string(ntohs(conn->getAddr()->addr.sin_port)));

    _timer->delTask(job);
  });
  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.insert_or_assign(conn->getSocket()->getFd(), conn);
  Log::debug("new connection, current connection count: %ld\n",
             _openConnection.size());
  Log::debug("client Port: ", ntohs(conn->getAddr()->addr.sin_port));
}

/// @brief conn断开连接时回调
/// @param conn
void Server::disConnection(Connection* conn) {
  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.erase(conn->getSocket()->getFd());
  Log::debug("dis connection, current connection count: ",
             _openConnection.size());
  Log::debug("client Port: ", ntohs(conn->getAddr()->addr.sin_port));
}
