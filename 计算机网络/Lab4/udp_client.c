#include "net.h"

char *msg[] = {"January","February","March","April",

"May","June","July","August","September","October",

"November","December"};

int main() {

    for(int i=0; i < 10; i++) {
        /* 建立套接字 */
        int sockfd = Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        /* 发送数据 */
        struct sockaddr_in server_addr = {0};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(UDP_SERVER_PORT);
        if (inet_pton(AF_INET, UDP_SERVER_ADDRESS, &server_addr.sin_addr.s_addr) != 1) {
            perror_and_exit("inet_pton");
        }

        Connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));

        printf("Send message: %s[%zu bytes]\n", msg[i], strlen(msg[i]));
        //Sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        Send(sockfd, msg[i], strlen(msg[i]), 0);

        /* 关闭套接字 */
        close(sockfd);
    }

    return 0;
}
