#include "Server.h"

Server::Server(const char* ip, uint16_t port) {
  _serverAddr = InetAddress(ip, port);
  init();
}

Server::Server(InetAddress serverAddr) : _serverAddr(serverAddr) {
  init();
}

void Server::init() {
  _loop = new EventLoop();
  _acceptor = new Acceptor(&_serverAddr);
  _acceptor->setNewConnection([=](Socket* servSocket, EventLoop* loop){
    printf("new connection\n");
    newConnection(servSocket, loop);
  });
}

Server::~Server() {
  delete _loop;
  _loop = nullptr;

  delete _acceptor;
  _acceptor = nullptr;
}

void Server::loop() {
  _acceptor->listen(_loop);
  printf("listen ...\n");

  _loop->start();
  _loop->loop();
}

void Server::newConnection(Socket* servSocket, EventLoop* loop) {
  // 1. 声明地址, 接收客户端连接
  InetAddress clientIp;
  int fd = servSocket->accept(&clientIp);
  Socket* client = new Socket(fd);
  client->setnonblocking();

  printf("new client fd %d! IP: %s Port: %d\n", client->getFd(),
         inet_ntoa(clientIp.addr.sin_addr), ntohs(clientIp.addr.sin_port));

  // 2. 构造 Channel, 放入 Eventloop
  Channel* channel = new Channel(fd);
  channel->prepareEvents();
  // 3. 对于(新增连接)客户端的回调, 也就是读取事件
  channel->setCallback([=]() { handleReadEvent(client, channel); });
  loop->updateChannel(channel);
}

void Server::handleReadEvent(Socket* socket, Channel* channel) {
  int fd = socket->getFd();
  char buf[READ_BUFFER];
  while (true) {
    // 由于使用非阻塞IO, 需要循环读取, 到这里时,
    // 数据可能没有传输完全, 如果判断数据不完整, 则等待一下continue
    bzero(&buf, sizeof(buf));
    ssize_t bytes_read = read(fd, buf, sizeof(buf));
    if (bytes_read > 0) {
      printf("message from client fd %d: %s\n", fd, buf);
      write(fd, buf,
            sizeof(bytes_read));  // 客户端发的数据echo回去, 读多少回复多少
    } else if (bytes_read == -1 &&
               errno == EINTR) {  // 客户端正常中断、继续读取
      printf("continue reading");
      continue;
    } else if (bytes_read == -1 &&
               (errno == EAGAIN ||
                errno ==
                    EWOULDBLOCK)) {  // 非阻塞IO，这个条件表示数据全部读取完毕
      printf("finish reading once, errno: %d\n", errno);
      break;
    } else if (bytes_read == 0) {  // EOF，客户端断开连接
      printf("EOF, client fd %d disconnected\n", fd);

      close(fd);  // 关闭socket会自动将文件描述符从epoll树上移除
      delete socket;  // 删除客户端
      delete channel;

      break;
    }
  }
}
