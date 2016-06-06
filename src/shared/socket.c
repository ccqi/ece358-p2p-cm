#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>

#include "socket.h"

void create_server(int8_t *sockfd, void *server, socklen_t *alen) {
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    // TODO: use mybind
    if (bind(*sockfd, (struct sockaddr *)server, sizeof(struct sockaddr)) < 0) {
        perror("could not bind to socket");
        exit(1);
    }

    *alen = sizeof(struct sockaddr_in);
    if (getsockname(*sockfd, (struct sockaddr *)server, alen) < 0) {
        perror("could not get socket name");
        exit(1);
    }
}
