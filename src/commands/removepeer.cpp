#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./removepeer <address> <port>\n");
        exit(1);
    }

    int8_t sockfd = -1;
    connect_to_server(&sockfd, argv[1], argv[2]);

    send_command(sockfd, (char *)"remove");

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_response(sockfd, buf);

    disconnect_from_server(sockfd);

    return 0;
}
