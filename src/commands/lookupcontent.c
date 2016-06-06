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

// TODO: put this in one place
#define SOCKET_TRANFER_LIMIT 16

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: ./lookupcontent <address> <port> <key>\n");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton((char *)argv[1], &(server.sin_addr));
    server.sin_port = htons(atoi(argv[2]));
    printf("Server: addr = %s, port = %d\n", (char *)argv[1], server.sin_port);

    int8_t sockfd = -1;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("could not open socket");
        exit(1);
    }

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = 0;
    if (bind(sockfd, (struct sockaddr *)&client, sizeof(struct sockaddr_in)) < 0) {
        perror("could not bind to socket");
        exit(1);
    }

    socklen_t alen = sizeof(struct sockaddr_in);
    if (getsockname(sockfd, (struct sockaddr *)&client, &alen) < 0) {
        perror("could not get socket name");
        exit(1);
    }

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
        perror("could not connect to server");
        exit(1);
    }

    size_t buflen = SOCKET_TRANFER_LIMIT;
    char buf[buflen];
    strcpy(buf, "hello");
    ssize_t sentlen;
    if ((sentlen = send(sockfd, buf, strlen(buf), 0)) < 0) {
        perror("could not send data to socket");
        exit(1);
    }

    buf[sentlen] = 0;
    printf("Sent %s to %s %d\n", buf, inet_ntoa(server.sin_addr),
           ntohs(server.sin_port));

    // print content specified by key
    bzero(buf, buflen);
    if (recv(sockfd, buf, buflen - 1, 0) < 0) {
        perror("error receiving response from server");
        exit(1);
    }

    printf("Received %s. Shutting down...\n", buf);

    if (shutdown(sockfd, SHUT_RDWR) < 0) {
        perror("error closing connection");
        exit(1);
    }

    return 0;
}
