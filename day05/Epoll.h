#pragma once
#include <sys/epoll.h>
#include <vector>
#include "util.h"
#include <cstring>
#include <unistd.h>
#include "Channel.h"

#define MAX_EVENTS 1000

class Channel;

class Epoll
{
private:
    int _epfd;
    struct epoll_event* _events;
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    std::vector<Channel*> poll(int timeout = -1);
    /**
     * @brief 更新channel
    */
    void updateChannel(Channel* channel);
};

