#include <arpa/inet.h>
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

#include "commands.h"
#include "content.h"
#include "nodes.h"
#include "state.h"

void join(char *ip, char *port, char *args[]) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, args[1], args[2]);

    std::stringstream ss;
    ss << "addpeer:" << ip << ":" << port;
    send_to_socket(sockfd, ss.str().c_str());

    disconnect_from_server(sockfd);
}

void init(int8_t *sockfd, socklen_t *alen, bool join_network, char *args[]) {
    char *ip = select_ip();

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = 0;

    create_server(sockfd, &server, alen);
    printf("%s %d\n", ip, ntohs(server.sin_port));
    fflush(stdout);

    char *port = new char[6];
    sprintf(port, "%d", ntohs(server.sin_port));
    self = left = right = addr_info(ip, port);

    total_peers = 1;
    total_content = 0;

    if (join_network) {
        join(ip, port, args);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        fprintf(stderr, "usage: ./peer [<address> <port>]\n");
        exit(1);
    }

    int8_t sockfd = -1;
    socklen_t alen = 0;
    init(&sockfd, &alen, argc == 3, argv);

    if (listen(sockfd, 5) < 0) {
        perror("error listening on socket");
        exit(1);
    }

    for (;;) {
        int8_t connectedsock;
        struct sockaddr_in client;
        alen = sizeof(struct sockaddr_in);
        if ((connectedsock =
                 accept(sockfd, (struct sockaddr *)&client, &alen)) < 0) {
            perror("could not accept connection");
            exit(1);
        }

        char buf[SOCKET_TRANSFER_LIMIT];
        receive_from_socket(connectedsock, buf);

        respond(strtok(buf, ":"), connectedsock);
    }

    if (close(sockfd) < 0) {
        perror("error closing socket as child");
    }

    return 0;
}
