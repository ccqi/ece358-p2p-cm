#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./removepeer <address> <port>\n");
        exit(1);
    }

    int8_t sockfd = -1;
    connect_to_socket(&sockfd, argv[1], argv[2]);

    std::stringstream ss;
    ss << "removepeer";
    send_to_socket(sockfd, ss.str().c_str());

    destroy_socket(sockfd);

    return 0;
}
