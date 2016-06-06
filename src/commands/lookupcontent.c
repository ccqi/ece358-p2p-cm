#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: ./lookupcontent <address> <port> <key>\n");
        exit(1);
    }

    int8_t sockfd = -1;
    connect_to_server(&sockfd, argv[1], argv[2]);

    send_command(sockfd, "hello");

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_response(sockfd, buf);

    disconnect_from_server(sockfd);

    return 0;
}
