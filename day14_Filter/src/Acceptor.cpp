#include "Acceptor.h"

Acceptor::Acceptor(InetAddress* serverAddr) {
  _serverSocket = new Socket();
  _serverSocket->bind(serverAddr);
  _serverSocket->setnonblocking();

  _serverChannel = new Channel(_serverSocket->getFd(), true);
  _serverChannel->inEvents();
}

Acceptor::~Acceptor() {
  delete _serverChannel;
  delete _serverSocket;
}

void Acceptor::setConn(std::function<void(Connection*)> fun) { _conn = fun; }

void Acceptor::listen(EventLoop* loop) {
  _serverSocket->listen();
  // 对于 服务socket的回调函数, 是新增一个连接
  _serverChannel->setReadCallback([=]() -> bool {
    InetAddress* addr = new InetAddress();
    int fd = _serverSocket->accept(addr);
    Connection* conn = new Connection(fd, addr);

    conn->getSocket()->setnonblocking();
    Channel* ch = conn->getChannel();
    loop->updateChannel(ch);  // 将连接的事件, 放到loop中
    _conn(conn);
    return false;
  });
  loop->updateChannel(_serverChannel);
}
