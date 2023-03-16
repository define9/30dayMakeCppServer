#include "Connection.h"

Connection::Connection(Socket* servSocket, EventLoop* loop) {
  // 1. 声明地址, 接收客户端连接
  _cltAddr = new InetAddress();
  int fd = servSocket->accept(_cltAddr);

  _cltSocket = new Socket(fd);
  _cltSocket->setnonblocking();

  // 2. 构造 Channel, 放入 Eventloop
  _cltChannel = new Channel(fd);
  _cltChannel->prepareEvents();

  // 3. 至此, 三样东西都已经new了,对于(新增连接)客户端的回调, 也就是读取事件
  _cltChannel->setCallback([=]() { handle(); });
  loop->updateChannel(_cltChannel);

  _inBuf = new Buffer();
  _outBuf = new Buffer();
}

Connection::~Connection() {
  delete _cltChannel;
  _cltChannel = nullptr;

  delete _cltAddr;
  _cltAddr = nullptr;

  delete _cltSocket;
  _cltSocket = nullptr;

  delete _inBuf;
  _inBuf = nullptr;

  delete _outBuf;
  _outBuf = nullptr;
}

void Connection::handle() {
  int fd = _cltSocket->getFd();
  char buf[PRE_READ_SIZE];
  while (true) {
    // 由于使用非阻塞IO, 需要循环读取, 到这里时,
    // 数据可能没有传输完全, 如果判断数据不完整, 则等待一下continue
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      _inBuf->append(buf, sizeof(buf));
    } else if (bytes_read == -1 &&
               errno == EINTR) {  // 客户端正常中断、继续读取
      continue;
    } else if (bytes_read == -1 &&
               (errno == EAGAIN ||
                errno ==
                    EWOULDBLOCK)) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("message from client: %s\n", _inBuf->c_str());
      errif(write(fd, _inBuf->c_str(), _inBuf->size()) < 0,
            "socket write error");
      _inBuf->clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", fd);
      _cb();
      break;
    }
  }
}

void Connection::setDisConnection(std::function<void()> cb) { _cb = cb; }

InetAddress* Connection::getCltAddr() { return _cltAddr; }

Socket* Connection::getCltSocket() { return _cltSocket; }
