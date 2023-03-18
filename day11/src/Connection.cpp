#include "Connection.h"

Connection::Connection(int fd, InetAddress* addr) {
  _addr = addr;
  _socket = new Socket(fd);
  init(fd);
}

Connection::Connection(Socket* socket, InetAddress* addr) {
  _addr = addr;
  _socket = socket;
  init(_socket->getFd());
}

Connection::~Connection() {
  delete _channel;
  delete _socket;
  delete _addr;
  delete _inBuf;
  delete _outBuf;
}

void Connection::init(int fd) {
  _socket->setnonblocking();

  _channel = new Channel(fd);
  _channel->inETEvents();
  _channel->setReadCallback([=]() { readHandle(); });
  _channel->setWriteCallback([=]() { writeHandle(); });

  _inBuf = new Buffer();
  _outBuf = new Buffer();
}

/// @brief 使用 epoll 监控了消息来的事件read
void Connection::readHandle() {
  int fd = _socket->getFd();
  char buf[PRE_READ_SIZE];
  while (true) {
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = ::read(fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      _inBuf->append(buf, sizeof(buf));
    } else if (bytes_read == -1 && errno == EINTR) {
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      if (_recvCb(_inBuf)) {
        _inBuf->clear();
      }
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", fd);
      _delCb();
      this->~Connection();
      return;
    }
  }
}

/// @brief 可能也需要加循环, 或者锁
void Connection::writeHandle() {
  int size = _outBuf->size();
  if (size > 0) {
    int fd = _socket->getFd();
    int res = ::write(fd, _outBuf->c_str(), _outBuf->size());
    if (res > 0) {
      printf("写入并清空\n");
      _outBuf->clear();
    }
  }
}

void Connection::setDisConnection(std::function<void()> cb) { _delCb = cb; }
void Connection::setRecvConnection(std::function<bool(Buffer* buf)> cb) {
  _recvCb = cb;
}

InetAddress* Connection::getAddr() { return _addr; }

Socket* Connection::getSocket() { return _socket; }

Channel* Connection::getChannel() { return _channel; }

int Connection::write(std::string str) {
  _outBuf->append(str.c_str(), str.length());
}
