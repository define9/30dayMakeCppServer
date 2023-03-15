#pragma once

#include <cstdio>
#include <vector>
#include "Epoll.h"
#include "Channel.h"

class EventLoop
{
private:
    Epoll* _ep;
    bool _running;
public:
    EventLoop();
    ~EventLoop();

    void loop();

    /**
     * 向 epoll 中更新 channel
    */
    void updateChannel(Channel* channel);

    void start();
    void stop();
};
