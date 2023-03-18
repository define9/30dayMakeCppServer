#include "Connection.h"

Connection::Connection(int fd, InetAddress* addr) {
  _addr = addr;
  _socket = new Socket(fd);
  _socket->setnonblocking();

  _channel = new Channel(fd);
  _channel->inETEvents();
  _channel->setCallback([=]() { handle(); });

  _inBuf = new Buffer();
  _outBuf = new Buffer();
}

Connection::~Connection() {
  printf("~Connection\n");
  delete _channel;
  delete _socket;
  delete _addr;
  delete _inBuf;
  delete _outBuf;
}

void Connection::handle() {
  int fd = _socket->getFd();
  char buf[PRE_READ_SIZE];
  while (true) {
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      _inBuf->append(buf, sizeof(buf));
    } else if (bytes_read == -1 && errno == EINTR) {
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      printf("message from client: %s\n", _inBuf->c_str());
      errif(write(fd, _inBuf->c_str(), _inBuf->size()) < 0,
            "socket write error");
      _inBuf->clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", fd);
      _cb();
      this->~Connection();
      return;
    }
  }
}

void Connection::setDisConnection(std::function<void()> cb) { _cb = cb; }

InetAddress* Connection::getAddr() { return _addr; }

Socket* Connection::getSocket() { return _socket; }

Channel* Connection::getChannel() { return _channel; }
