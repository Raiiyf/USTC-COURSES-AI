#include "net.h"

int main() {

    /* 建立套接字 */
    int sockfd = Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* 建立tcp连接 */
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(TCP_SERVER_PORT);
    if (inet_pton(AF_INET, TCP_SERVER_ADDRESS, &server_addr.sin_addr.s_addr) != 1) {
        perror_and_exit("inet_pton");
    }

    Connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

    /* 发送数据 */
    char msg[1024];
    scanf("%s", msg);
    //printf("Send message: %s[%zu bytes]\n", msg, strlen(msg));
    Send(sockfd, msg, strlen(msg), 0);

    char buf_client[TCP_BUF_LENGTH];
    size_t pkt_len = Recv(sockfd, buf_client, TCP_BUF_LENGTH, 0);
    if (pkt_len > 0) {
        buf_client[pkt_len] = '\0';
        printf("Message received from server: %s[%zu bytes]\n", buf_client, pkt_len);
    } else {
        printf("Connection closed\n");
    }

    /* 关闭套接字 */
    close(sockfd);

    return 0;
}
