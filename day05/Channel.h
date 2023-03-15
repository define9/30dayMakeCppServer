#pragma once

#include <unistd.h>
#include "Epoll.h"

class Epoll;

class Channel
{
private:
    Epoll* _ep;
    int _fd;
    uint32_t _events;
    uint32_t _revents;
    bool _inEpoll;

public:
    Channel(int fd, Epoll* ep);
    ~Channel();

    int getFd();
    uint32_t getEvents();
    bool getInEpoll();
    void addEpoll();

    void setEvents(uint32_t events);

    void enableReading();
};
