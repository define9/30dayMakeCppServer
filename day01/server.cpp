#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); // 0.0.0.0
    serveraddr.sin_port = htons(8888);

    bind(sockfd, (sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(sockfd, SOMAXCONN);
    printf("listen\n");

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, sizeof(client_addr));
    int client_sockfd = accept(sockfd, (sockaddr*)&client_addr, &client_addr_len);
    printf("new client fd %d! IP: %s Port: %d\n", client_sockfd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    printf("end\n");
    close(sockfd);
}