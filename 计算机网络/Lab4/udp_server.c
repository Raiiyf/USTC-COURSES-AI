#include "net.h"

int main() {

    /* 建立套接字 */
    int sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    int enable = 1;
    Setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));

    /* 绑定端口 */
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(UDP_SERVER_PORT);
    if (inet_pton(AF_INET, UDP_SERVER_ADDRESS, &server_addr.sin_addr.s_addr) != 1) {
        perror_and_exit("inet_pton");
    }

    Bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    int n = 0;
    while(n < 10) {
        /* 接收数据 */
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);

        char buf[UDP_BUF_LENGTH];
        size_t pkt_len = Recvfrom(sockfd, buf, UDP_BUF_LENGTH, 0, (struct sockaddr *)&client_addr, &client_addr_len);

        char client_address[100];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_address, sizeof(client_address));
        int client_port = ntohs(client_addr.sin_port);

        buf[pkt_len] = '\0';
        printf("%d: Message received from %s, %d: %s[%zu bytes]\n", n + 1, client_address, client_port, buf, pkt_len);

        n++;
    }

    /* 关闭套接字 */
    close(sockfd);

    return 0;
}
