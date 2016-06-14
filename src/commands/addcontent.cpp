#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: ./addcontent <address> <port> <value>\n");
        exit(1);
    }

    int8_t sockfd = -1;
    connect_to_socket(&sockfd, argv[1], argv[2]);

    std::stringstream ss;
    ss << "addcontent:" << argv[3];
    send_to_socket(sockfd, ss.str().c_str());

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_from_socket(sockfd, buf);

    if (strcmp(buf, "-") == 0) {
        fprintf(stderr, "Error: no space for new content\n");
        destroy_socket(sockfd);
        return 1;
    }

    printf("%s\n", buf);
    destroy_socket(sockfd);
    return 0;
}
