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

#include "content.h"
#include "nodes.h"

void join(char *ip, char *port, char *args[]) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, args[1], args[2]);

    std::stringstream ss;
    ss << "newpeer:" << ip << ":" << port;
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
    init_node(ip, port);

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

        char *command = strdup(strtok(buf, ":"));

        if (strcmp(command, "insert") == 0) {
            char *val = strdup(strtok(NULL, ":"));
            uint8_t key = insert_content(val);
            // TODO: set a reasonable buffer size for key
            char *keyChar = new char[6];
            sprintf(keyChar, "%d", ntohs(key));
            send_to_socket(connectedsock, keyChar);
        } else if (strcmp(command, "lookup") == 0) {
            char *key = strdup(strtok(NULL, ":"));
            char *value = read_content(atoi(key));
            send_to_socket(connectedsock, value);
        } else if (strcmp(buf, "delete") == 0) {
            delete_content(atoi(strdup(strtok(NULL, ":"))));
        } else if (strcmp(buf, "newpeer") == 0) {
            char *ip = strdup(strtok(NULL, ":"));
            char *port = strdup(strtok(NULL, ":"));
            add_node(ip, port);
        } else if (strcmp(buf, "removepeer") == 0) {
            char *ip = strdup(strtok(NULL, ":"));
            char *port = strdup(strtok(NULL, ":"));
            remove_self(ip, port);
        } else if (strcmp(buf, "removenode") == 0) {
            char *ipRemove = strdup(strtok(NULL, ":"));
            char *portRemove = strdup(strtok(NULL, ":"));
            char *ipLeft = strdup(strtok(NULL, ":"));
            char *portLeft = strdup(strtok(NULL, ":"));
            char *ipRight = strdup(strtok(NULL, ":"));
            char *portRight = strdup(strtok(NULL, ":"));
            remove_node(ipRemove, portRemove, ipLeft, portLeft, ipRight,
                        portRight);
        } else if (strcmp(buf, "clonepeer") == 0) {
            int total_peers = atoi(strdup(strtok(NULL, ":")));
            int total_content = atoi(strdup(strtok(NULL, ":")));
            char *ipLeft = strdup(strtok(NULL, ":"));
            char *portLeft = strdup(strtok(NULL, ":"));
            char *ipRight = strdup(strtok(NULL, ":"));
            char *portRight = strdup(strtok(NULL, ":"));
            clone_node(total_peers, total_content, ipLeft, portLeft, ipRight,
                       portRight);
        } else if (strcmp(buf, "connectnewpeer") == 0) {
            char *ipReplace = strdup(strtok(NULL, ":"));
            char *portReplace = strdup(strtok(NULL, ":"));
            char *ipNew = strdup(strtok(NULL, ":"));
            char *portNew = strdup(strtok(NULL, ":"));
            connect_node(ipReplace, portReplace, ipNew, portNew);
        } else if (strcmp(buf, "debug") == 0) {
            debug_node();
            debug_content((char *)"peer debug");
        } else {
            // TODO: some sort of error
        }
        // TODO: Error cases
        debug_content((char *)"end of for");
    }

    if (close(sockfd) < 0) {
        perror("error closing socket as child");
    }

    return 0;
}
