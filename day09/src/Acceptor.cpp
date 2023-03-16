#include "Acceptor.h"

Acceptor::Acceptor(InetAddress* serverAddr)
{
  _serverSocket = new Socket();
  _serverSocket->bind(serverAddr);
  _serverSocket->setnonblocking();
  
  _serverChannel = new Channel(_serverSocket->getFd());
  _serverChannel->prepareEvents();
}

Acceptor::~Acceptor()
{
  delete _serverSocket;
  _serverSocket = nullptr;

  delete _serverChannel;
  _serverChannel = nullptr;
}

void Acceptor::setNewConnection(std::function<void(Socket* servSocket, EventLoop* loop)> fun) {
  _newConnection = fun;
}

void Acceptor::listen(EventLoop* loop) {
  _serverSocket->listen();
  // 对于 服务socket的回调函数, 是新增一个连接
  _serverChannel->setCallback([=]() { _newConnection(_serverSocket, loop); });
  loop->updateChannel(_serverChannel);
}