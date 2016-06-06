#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../shared/socket.h"

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        printf("usage: ./peer [<address> <port>]\n");
        exit(1);
    }

    // --- Select IP
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
            memcpy(&srv_ip, &(((struct sockaddr_in *)(i->ifa_addr))->sin_addr), sizeof(struct in_addr));
            ip = inet_ntoa(srv_ip);
            break;
        }
    }

    freeifaddrs(ifa);
    // ---

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = 0;

    int8_t sockfd = -1;
    socklen_t alen = 0;
    create_server(&sockfd, &server, &alen);
    printf("%s %d\n", ip, server.sin_port);

    if (argc == 3) {
        printf("Gonna send to %s %s\n", argv[1], argv[2]);
        // TODO
    }

    for (;;) {
        if (listen(sockfd, 0) < 0) {
            perror("error listening on socket");
            exit(1);
        }

        uint8_t connectedsock;
        struct sockaddr_in client;
        alen = sizeof(struct sockaddr_in);
        if ((connectedsock = accept(sockfd, (struct sockaddr *)&client, &alen)) < 0) {
            perror("could not accept connection");
            exit(1);
        }

        if (fork()) {
            // parent
            if (close(connectedsock) < 0) {
                perror("error closing socket as server");
            }

            continue;
        } else {
            // child
            if (close(sockfd) < 0) {
                perror("error closing socket as child");
            }

            size_t buflen = SOCKET_TRANSFER_LIMIT;
            char buf[buflen];
            ssize_t recvlen;
            if ((recvlen = recv(connectedsock, buf, buflen - 1, 0)) < 0) {
                perror("could not receive on socket");
                exit(1);
            }

            buf[recvlen] = 0;
            // do the things with the stuff
            printf("Child %d received the following %d-length string: %s\n", getpid(), (int)recvlen, buf);

            // respond:
            strcpy(buf, "bye");
            printf("Child %d sending %s\n", getpid(), buf);
            if (send(connectedsock, buf, strlen(buf), 0) < 0) {
                perror("send");
                return -1;
            }

            if (shutdown(connectedsock, SHUT_RDWR) < 0) {
                perror("could not shutdown client connection");
                exit(1);
            }

            return 0;
        }
    }
}
