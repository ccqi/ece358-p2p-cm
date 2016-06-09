#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sstream>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared/address.h"
#include "../shared/socket.h"

#include "content.h"

void join(char *ip, uint8_t port, char *args[]) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, args[1], args[2]);

    std::stringstream ss;
    ss << "newpeer:" << ip << ":" << port;
    send_to_socket(sockfd, ss.str().c_str());

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_from_socket(sockfd, buf); // TODO: receives peer list

    disconnect_from_server(sockfd);
}

void init(int8_t *sockfd, socklen_t *alen, bool join_network, char *args[]) {
    char *ip = select_ip();

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = 0;

    create_server(sockfd, &server, alen);
    printf("%s %d\n", ip, server.sin_port);

    if (join_network) {
        join(ip, server.sin_port, args);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        printf("usage: ./peer [<address> <port>]\n");
        exit(1);
    }

    int8_t sockfd = -1;
    socklen_t alen = 0;
    init(&sockfd, &alen, argc == 3, argv);

    for (;;) {
        if (listen(sockfd, 0) < 0) {
            perror("error listening on socket");
            exit(1);
        }

        uint8_t connectedsock;
        struct sockaddr_in client;
        alen = sizeof(struct sockaddr_in);
        if ((connectedsock =
                 accept(sockfd, (struct sockaddr *)&client, &alen)) < 0) {
            perror("could not accept connection");
            exit(1);
        }

        if (fork()) {
            if (close(connectedsock) < 0) {
                perror("error closing socket as parent");
            }

            continue;
        }

        if (close(sockfd) < 0) {
            perror("error closing socket as child");
        }

        char buf[SOCKET_TRANSFER_LIMIT];
        receive_from_socket(connectedsock, buf);

        char *command = strtok(buf, ":");
        if (strcmp(command, "insert") == 0) {
            char *key = insert_content(strtok(NULL, ":"));
            send_to_socket(connectedsock, key);
        } else if (strcmp(command, "lookup") == 0) {
            char *value = read_content(strtok(NULL, ":"));
            send_to_socket(connectedsock, value);
        } else if (strcmp(buf, "delete") == 0) {
            delete_content(strtok(NULL, ":"));
        } else if (strcmp(buf, "remove") == 0) {
            // remove self from network
        } else if (strcmp(buf, "newpeer") == 0) {
            char *ip = strtok(NULL, ":");
            char *port = strtok(NULL, ":");
            (void)ip;
            (void)port;
            // add peer with ip and port to network
        } else if (strcmp(buf, "removepeer") == 0) {
            char *ip = strtok(NULL, ":");
            char *port = strtok(NULL, ":");
            (void)ip;
            (void)port;
            // remove peer with ip and port to network
        } else {
            // TODO: some sort of error
        }
        // TODO: Error cases

        disconnect_from_server(connectedsock);

        return 0;
    }
}
