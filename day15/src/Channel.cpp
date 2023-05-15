#include "Channel.h"

Channel::Channel(int fd, bool useSync) : _fd(fd), _useSync(useSync) {
  _inEpoll = false;
}

Channel::~Channel() {}

void Channel::inETEvents() { _events = EPOLLIN | EPOLLET; }
void Channel::inEvents() { _events = EPOLLIN; }

int Channel::getFd() { return _fd; }
uint32_t Channel::getEvents() { return _events; }
uint32_t Channel::getRevents() { return _revents; }
bool Channel::getInEpoll() { return _inEpoll; }
bool Channel::isSync() { return _useSync; }
void Channel::addToEpoll() { _inEpoll = true; }
void Channel::deleteFromEpoll() { _inEpoll = false; }

void Channel::setRevents(uint32_t revents) { _revents = revents; }

void Channel::setCallback(std::function<void()> callback) {
  _callback = callback;
}

void Channel::handle() { _callback(); }