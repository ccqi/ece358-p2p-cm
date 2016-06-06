#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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

void connect_to_server(int8_t *sockfd, char *ip, char *port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, &(server.sin_addr));
    server.sin_port = atoi(port);

    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = 0;
    if (bind(*sockfd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) <
        0) {
        perror("could not bind to socket");
        exit(1);
    }

    socklen_t alen = sizeof(struct sockaddr_in);
    if (getsockname(*sockfd, (struct sockaddr *)&client, &alen) < 0) {
        perror("could not get socket name");
        exit(1);
    }

    if (connect(*sockfd, (struct sockaddr *)&server,
                sizeof(struct sockaddr_in)) < 0) {
        perror("could not connect to server");
        exit(1);
    }
}

void disconnect_from_server(uint8_t sockfd) {
    if (shutdown(sockfd, SHUT_RDWR) < 0) {
        perror("error closing connection");
        exit(1);
    }
}

void send_command(int8_t sockfd, char *command) {
    char buf[SOCKET_TRANSFER_LIMIT];
    strncpy(buf, command, SOCKET_TRANSFER_LIMIT);
    buf[SOCKET_TRANSFER_LIMIT - 1] = 0;

    if (send(sockfd, buf, strlen(buf), 0) < 0) {
        perror("could not send data to socket");
        exit(1);
    }
}

void receive_response(int8_t sockfd, char *response) {
    ssize_t recvlen;
    if ((recvlen = recv(sockfd, response, SOCKET_TRANSFER_LIMIT - 1, 0)) < 0) {
        perror("error receiving response from server");
        exit(1);
    }

    response[recvlen] = 0;
}
