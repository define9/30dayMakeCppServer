#include "Acceptor.h"

/// @brief 非堵塞监听可读事件，建立连接
/// @param serverAddr 监听的地址
Acceptor::Acceptor(InetAddress* serverAddr) {
  _serverSocket = new Socket();
  _serverSocket->bind(serverAddr);
  _serverSocket->setnonblocking();

  _serverChannel = new Channel(_serverSocket->getFd(), true);
  _serverChannel->inEvents();  // 对于服务的socket，水平触发

  _timer = new Timer(0);
}

Acceptor::~Acceptor() {
  delete _serverSocket;
  delete _serverChannel;
  delete _timer;
}

void Acceptor::setConn(std::function<void(Connection*)> fun) { _conn = fun; }

void Acceptor::listen(EventLoop* loop) {
  _serverSocket->listen();
  // 对于 服务socket的回调函数, 是新增一个连接
  _serverChannel->setCallback([=]() {
    InetAddress* addr = new InetAddress();
    int fd = _serverSocket->accept(addr);
    Connection* conn = new Connection(fd, addr);

    Channel* ch = conn->getChannel();
    loop->updateChannel(ch);  // 将连接的事件, 放到loop中
    _conn(conn);
  });
  loop->updateChannel(_serverChannel);
  loop->updateChannel(_timer->getChannel());

  _timer->addTask(10, [=]() { Log::debug("我是10s后的任务"); });
  _timer->addTask(20, [=]() {
    Log::debug("我是20s后的任务,我添加一个10s后的任务");
    _timer->addTask(10,
                    [=]() { Log::debug("我是20s后添加的一个10s后的任务"); });
  });
  _timer->addTask(61, [=]() { Log::debug("我是61s后的任务"); });
  _timer->addTask(63, [=]() { Log::debug("我是63s后的任务"); });
}
