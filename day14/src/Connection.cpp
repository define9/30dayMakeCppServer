#include "Connection.h"

Connection::Connection(int fd, InetAddress* addr) {
  _addr = addr;
  _socket = new Socket(fd);
  _socket->setnonblocking();

  _channel = new Channel(fd);
  _channel->inETEvents();  // 对于客户端的连接监听，边沿触发
  _channel->setCallback([=]() { handle(); });  // socket边沿触发回调

  _inBuf = new Buffer();
  _outBuf = new Buffer();
}

Connection::~Connection() {
  Log::debug("~Connection");
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
      Log::debug("message from client: ", _inBuf->c_str());

      // 接收到了客户端传的全部数据
      _handle(_inBuf, _outBuf);
      // 已经得到了应该返回的

      _inBuf->clear();
      errif(write(fd, _outBuf->c_str(), _outBuf->size()) < 0,
            "socket write error");
      _outBuf->clear();
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      Log::debug("EOF, client disconnected, fd: ", fd);
      disConnect();
      return;
    }
  }
}

void Connection::setHandle(
    std::function<void(Buffer* in, Buffer* out)> handle) {
  _handle = handle;
}
void Connection::setDisConnection(std::function<void()> cb) { _cb = cb; }
void Connection::disConnect() {
  _cb();
  this->~Connection();
}
InetAddress* Connection::getAddr() { return _addr; }

Socket* Connection::getSocket() { return _socket; }

Channel* Connection::getChannel() { return _channel; }
