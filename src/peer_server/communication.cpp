#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/socket.h"

#include "communication.h"

void forward(const char *message) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, right.ip, right.port);
    send_to_socket(sockfd, message);
    disconnect_from_server(sockfd);
}

void forward(const char *message, const char *origin_ip,
             const char *origin_port) {
    if (strcmp(right.ip, origin_ip) == 0 &&
        strcmp(right.port, origin_port) == 0) {
        return;
    }

    forward(message);
}

char *select_ip() {
    char *ip = 0;

    struct ifaddrs *ifa;
    if (getifaddrs(&ifa) < 0) {
        perror("could not get host addresses");
        exit(1);
    }

    struct in_addr srv_ip;
    for (struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next) {
        if (i->ifa_addr == NULL) {
            continue;
        }

        if (i->ifa_addr->sa_family == AF_INET) {
            memcpy(&srv_ip, &(((struct sockaddr_in *)(i->ifa_addr))->sin_addr),
                   sizeof(struct in_addr));
            ip = inet_ntoa(srv_ip);
            break;
        }
    }

    freeifaddrs(ifa);
    return ip;
}
