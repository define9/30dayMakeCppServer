#include "Channel.h"

Channel::Channel(int fd) : _fd(fd) {}

Channel::~Channel() {
  if (_fd >= 0) {
    close(_fd);
    _fd = -1;
  }
}

void Channel::prepareEvents() { _events = EPOLLIN | EPOLLET; }

int Channel::getFd() { return _fd; }
uint32_t Channel::getEvents() { return _events; }
uint32_t Channel::getRevents() { return _revents; }
bool Channel::getInEpoll() { return _inEpoll; }
void Channel::addEpoll() { _inEpoll = true; }

void Channel::setRevents(uint32_t revents) { _revents = revents; }

void Channel::setCallback(std::function<void()> callback) {
  _callback = callback;
}

void Channel::handle() { _callback(); }