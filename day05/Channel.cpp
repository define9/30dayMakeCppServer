#include "Channel.h"

Channel::Channel(int fd, Epoll* ep): _fd(fd), _ep(ep)
{
}

Channel::~Channel()
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
    delete _ep;
    _ep = nullptr;
}

void Channel::enableReading() {
    _events = EPOLLIN | EPOLLET;
    _ep->updateChannel(this);
}

int Channel::getFd() { return _fd; }
uint32_t Channel::getEvents() { return _events; }
bool Channel::getInEpoll() { return _inEpoll; }
void Channel::addEpoll() { _inEpoll = true; }

void Channel::setEvents(uint32_t events) {
    _events = events;
}