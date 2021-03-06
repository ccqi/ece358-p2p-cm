#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "socket.h"

int mybind(int sockfd, struct sockaddr_in *addr) {
    if (sockfd < 1) {
        fprintf(stderr, "mybind(): sockfd has invalid value %d\n", sockfd);
        return -1;
    }

    if (addr == NULL) {
        fprintf(stderr, "mybind(): addr is NULL\n");
        return -1;
    }

    unsigned short p;
    for (p = PORT_RANGE_LO; p <= PORT_RANGE_HI; p++) {
        addr->sin_port = htons(p);
        if (bind(sockfd, (const struct sockaddr *)addr,
                 sizeof(struct sockaddr_in)) >= 0) {
            break;
        }
    }

    if (p > PORT_RANGE_HI) {
        fprintf(
            stderr,
            "mybind(): all bind() attempts failed. No port available...?\n");
        return -1;
    }

    return 0;
}

void create_server(int8_t *sockfd, void *server, socklen_t *alen) {
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    if (mybind(*sockfd, (struct sockaddr_in *)server) < 0) {
        perror("could not bind to socket");
        exit(1);
    }

    *alen = sizeof(struct sockaddr_in);
    if (getsockname(*sockfd, (struct sockaddr *)server, alen) < 0) {
        perror("could not get socket name");
        exit(1);
    }
}

void connect_to_socket(int8_t *sockfd, char *ip, char *port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, &(server.sin_addr));
    server.sin_port = htons(atoi(port));

    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    if (connect(*sockfd, (struct sockaddr *)&server,
                sizeof(struct sockaddr_in)) < 0) {
        if (errno == ECONNREFUSED) {
            fprintf(stderr, "Error: no such peer\n");
        } else {
            perror("could not connect to server");
        }
        exit(1);
    }
}

void destroy_socket(int8_t sockfd) {
    if (close(sockfd) < 0) {
        perror("error destroying socket");
        exit(1);
    }
}

void send_to_socket(int8_t sockfd, const char *command) {
    char buf[SOCKET_TRANSFER_LIMIT];
    strncpy(buf, command, SOCKET_TRANSFER_LIMIT);
    buf[SOCKET_TRANSFER_LIMIT - 1] = 0;

    if (send(sockfd, buf, strlen(buf), 0) < 0) {
        perror("could not send data to socket");
        exit(1);
    }
}

void receive_from_socket(int8_t sockfd, char *response) {
    ssize_t recvlen;
    if ((recvlen = recv(sockfd, response, SOCKET_TRANSFER_LIMIT - 1, 0)) < 0) {
        perror("error receiving response from server");
        exit(1);
    }

    response[recvlen] = 0;
}
