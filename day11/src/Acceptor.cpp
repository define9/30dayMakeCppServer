#include "Acceptor.h"

/// @brief 非堵塞监听可读事件，建立连接
/// @param serverAddr 监听的地址
Acceptor::Acceptor(InetAddress* serverAddr)
{
  _serverSocket = new Socket();
  _serverSocket->bind(serverAddr);
  _serverSocket->setnonblocking();
  
  _serverChannel = new Channel(_serverSocket->getFd(), true);
  _serverChannel->inEvents();
}

Acceptor::~Acceptor()
{
  delete _serverSocket;
  delete _serverChannel;
}

void Acceptor::setConn(std::function<void(Connection*)> fun) {
  _conn = fun;
}

void Acceptor::listen(EventLoop* loop) {
  _serverSocket->listen();
  // 对于 服务socket的回调函数, 是新增一个连接
  _serverChannel->setCallback([=]() {
    InetAddress* addr = new InetAddress();
    int fd = _serverSocket->accept(addr);
    Connection* conn = new Connection(fd, addr);
    
    Channel* ch = conn->getChannel();
    loop->updateChannel(ch); //将连接的事件, 放到loop中
    _conn(conn);
  });
  loop->updateChannel(_serverChannel);
}
