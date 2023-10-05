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
  _dispatcher = new Dispatcher();

  // 定时器加入到epoll中
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
  delete _dispatcher;
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
    auto job = _timer->addTask(60, [=]() {
      Log::debug("timeout...");
      conn->disConnect();
    });

    // 把输入缓存解析成 Http请求
    Request* req = new Request();
    RequestBuilder::build(req, in);
    Response* resp = new Response(req);

    // 负责解决请求和响应
    _dispatcher->resolve(req, resp);

    // 将响应写到输出缓存,然后写入
    out->append(ResponseBuilder::serialize(resp));
    _timer->delTask(job);
    delete req;
    delete resp;
  });
  std::unique_lock<std::mutex> lock(_mapLock);
  _openConnection.insert_or_assign(conn->getSocket()->getFd(), conn);
  Log::debug("new connection, current connection count: ",
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

void Server::get(const std::string& url, HandleFun fun) {
  _dispatcher->addHandle(url, ReqMethod::GET, fun);
}

void Server::post(const std::string& url, HandleFun fun) {
  _dispatcher->addHandle(url, ReqMethod::POST, fun);
}

void Server::mount(const std::string& path, const std::string& dir) {
  _dispatcher->mount(path, dir);
}