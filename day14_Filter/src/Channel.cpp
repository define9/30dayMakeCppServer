#include "Channel.h"

Channel::Channel(int fd, bool useSync) : _fd(fd), _useSync(useSync) {
  _inEpoll = false;
}

Channel::~Channel() {}

void Channel::inETEvents() { _events = EPOLLIN | EPOLLOUT | EPOLLET; }
void Channel::inEvents() { _events = EPOLLIN | EPOLLOUT; }

int Channel::getFd() { return _fd; }
uint32_t Channel::getEvents() { return _events; }
uint32_t Channel::getRevents() { return _revents; }
bool Channel::getInEpoll() { return _inEpoll; }
bool Channel::isSync() { return _useSync; }
void Channel::addToEpoll() { _inEpoll = true; }
void Channel::deleteFromEpoll() { _inEpoll = false; }

void Channel::setRevents(uint32_t revents) { _revents = revents; }

void Channel::setReadCallback(std::function<bool()> read) {
  _read_callback = read;
}
void Channel::setWriteCallback(std::function<void()> write) {
  _write_callback = write;
}

void Channel::handle() {
  bool close = false;
  if (_revents & (EPOLLIN | EPOLLPRI)) {
    close = _read_callback();
  }
  if (!close && _revents & (EPOLLOUT)) {
    _write_callback();
  }
}