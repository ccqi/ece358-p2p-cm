#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: ./lookupcontent <address> <port> <key>\n");
        exit(1);
    }

    int8_t sockfd = -1;
    connect_to_server(&sockfd, argv[1], argv[2]);

    std::stringstream ss;
    ss << "lookup:" << argv[3];
    send_to_socket(sockfd, ss.str().c_str());

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_from_socket(sockfd, buf);

    printf("%s\n", buf);

    disconnect_from_server(sockfd);

    return 0;
}
