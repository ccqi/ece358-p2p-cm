#include <arpa/inet.h>
#include <errno.h>
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

#define SOCKET_TRANSFER_LIMIT 16

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./peer <address> <port>\n");
        printf("%d %s %s", argc, argv[0], argv[1]);
        exit(1);
    }

    int8_t sockfd = -1;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton((char *)argv[1], (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = htons(atoi(argv[2]));
    printf("Binding on %d\n", server.sin_port);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
        perror("could not bind to socket");
        exit(1);
    }

    socklen_t alen = sizeof(struct sockaddr_in);
    if (getsockname(sockfd, (struct sockaddr *)&server, &alen) < 0) {
        perror("could not get socket name");
        exit(1);
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
