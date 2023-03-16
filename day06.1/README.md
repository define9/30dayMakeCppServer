### 2023/03/15
抽象出 EventLoop, 事件驱动
应该是没有内存泄漏的

可以向 EventLoop 中注册事件(updateChannel)
loop 负责主循环, 当从epoll中发现事件, 并同步调用事件的回调函数(callback)
后续出线程池后, 可以异步调用回调函数