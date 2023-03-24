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
  _channel = new Channel(fd);
  _channel->inETEvents();
  _channel->setReadCallback([=]() { return readHandle(); });
  _channel->setWriteCallback([=]() { writeHandle(); });

  _inBuf = new Buffer();
  _outBuf = new Buffer();
}

/// @brief 使用 epoll 监控了消息来的事件read
bool Connection::readHandle() {
  if (_socket->isNonBlock()) {
    return readNonBlock();
  } else {
    return readBlock();
  }
}

void Connection::writeHandle() {
  Log::debug("outBuf write and clear");
  if (_socket->isNonBlock()) {
    writeNonBlock();
  } else {
    writeBlock();
  }
  _outBuf->clear();
}

void Connection::setDisConnection(std::function<void()> cb) { _delCb = cb; }
void Connection::setRecvConnection(std::function<bool(Buffer* buf)> cb) {
  _recvCb = cb;
}

InetAddress* Connection::getAddr() { return _addr; }

Socket* Connection::getSocket() { return _socket; }

Channel* Connection::getChannel() { return _channel; }

void Connection::write(std::string str, bool force) {
  _outBuf->append(str.c_str(), str.length());
  if (force) {
    writeHandle();
  }
}

std::string Connection::read(bool force) {
  if (force) {
    readHandle();
  }
  return _inBuf->c_str();
}

bool Connection::readNonBlock() {
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
      if (_recvCb(_inBuf) != true) {
        _inBuf->clear();
      }
      break;
    } else if (bytes_read == 0) {  // EOF, 断开连接
      _delCb();
      goto kill;
    }
  }
  return false;
kill:
  this->~Connection();
  return true;
}
bool Connection::readBlock() {
  int fd = _socket->getFd();
  unsigned int rcv_size = 0;
  socklen_t len = sizeof(rcv_size);
  getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &len);
  char buf[rcv_size];
  ssize_t bytes_read = ::read(fd, buf, sizeof(buf));
  if (bytes_read > 0) {
    _inBuf->append(buf, bytes_read);
  } else if (bytes_read == 0) {
    Log::debug("read EOF, blocking client fd: ", fd);
    _delCb();
    goto kill;
  } else if (bytes_read == -1) {
    Log::debug("Other error on blocking client fd: ", fd);
  }
  if (_recvCb(_inBuf) != true) {
    _inBuf->clear();
  }
  return false;
kill:
  this->~Connection();
  return true;
}

void Connection::writeNonBlock() {
  int fd = _socket->getFd();
  char buf[_outBuf->size()];
  memcpy(buf, _outBuf->c_str(), _outBuf->size());
  int data_size = _outBuf->size();
  int data_left = data_size;
  while (data_left > 0) {
    ssize_t bytes_write = ::write(fd, buf + data_size - data_left, data_left);
    if (bytes_write == -1 && errno == EINTR) {
      continue;
    }
    if (bytes_write == -1 && errno == EAGAIN) {
      break;
    }
    if (bytes_write == -1) {
      Log::debug("Other error on blocking client fd: ", fd);
      break;
    }
    data_left -= bytes_write;
  }
}
void Connection::writeBlock() {
  // 没有处理send_buffer_数据大于TCP写缓冲区，的情况，可能会有bug
  int fd = _socket->getFd();
  ssize_t bytes_write = ::write(fd, _outBuf->c_str(), _outBuf->size());
  if (bytes_write == -1) {
    Log::debug("Other error on blocking client fd: ", fd);
  }
}
