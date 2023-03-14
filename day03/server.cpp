#include <stdio.h>
#include <sys/socket.h>//创建socket
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>//设置非阻塞
#include <sys/epoll.h>//epoll
#include <errno.h>
#include "util.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main() {
    // 1. 创建服务端socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd < 0, "socket create error\n");

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    serveraddr.sin_port = htons(8888);

    // 2. 绑定ip和端口
    int res = bind(sockfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
    errif(res < 0, "bind addr error\n");
    // 3. 开始监听
    res = listen(sockfd, SOMAXCONN);
    errif(res < 0, "listen error\n");
    printf("listen\n");

    // 4. 创建epoll, 处理socket, 这里size传1就行
    int epfd = epoll_create(1);
    errif(epfd < 0, "epoll create error\n");
    struct epoll_event events[MAX_EVENTS], ev;
    bzero(&events, sizeof(events));
    bzero(&ev, sizeof(ev));
    ev.data.fd = sockfd;
    ev.events = EPOLLIN; // 可读, 服务端socket, 只负责接收请求, 不使用边缘触发

    setnonblocking(sockfd); // 设置非堵塞
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); // 添加到epoll

    while(true) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1); //有nfds个事件
        errif(nfds == -1, "epoll wait error\n");
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == sockfd) { // 事件是服务端
                if (events[i].events & EPOLLIN) {
                    struct sockaddr_in client_addr;
                    socklen_t client_addr_len = sizeof(client_addr);
                    memset(&client_addr, 0, sizeof(client_addr));
                    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
                    errif(client_sockfd < 0, "accept error\n");

                    printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    bzero(&ev, sizeof(ev));
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = client_sockfd;
                    setnonblocking(client_sockfd);
                    epoll_ctl(epfd, EPOLL_CTL_ADD, client_sockfd, &ev);
                }
            } else if (events[i].events & EPOLLIN) { // 可读事件, 传递了数据
                char buf[READ_BUFFER];
                while(true) { //由于使用非阻塞IO, 需要循环读取, 到这里时, 数据可能没有传输完全, 如果判断数据不完整, 则等待一下continue
                    bzero(&buf, sizeof(buf));
                    ssize_t bytes_read = read(events[i].data.fd, buf, sizeof(buf));
                    if(bytes_read > 0){
                        printf("message from client fd %d: %s\n", events[i].data.fd, buf);
                        write(events[i].data.fd, buf, sizeof(bytes_read)); // 客户端发的数据echo回去, 读多少回复多少
                    } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
                        printf("continue reading");
                        continue;
                    } else if(bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)){//非阻塞IO，这个条件表示数据全部读取完毕
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    } else if(bytes_read == 0){  //EOF，客户端断开连接
                        printf("EOF, client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd); //关闭socket会自动将文件描述符从epoll树上移除
                        break;
                    }
                }
            } else {
                printf("event unsupport\n");
            }
        }
    }

    printf("end\n");
    close(sockfd);
}