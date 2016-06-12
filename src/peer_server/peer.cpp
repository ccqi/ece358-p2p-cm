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

struct addrInfo {
  char *ip;
  uint8_t port;

  addrInfo(char *_ip, uint8_t _port) { ip = _ip; port = _port; }
  addrInfo() {}
} selfInfo, left, right;

int totalPeers;
int totalContent;

void addPeer(char *ip, uint8_t port) {
  if (right.ip == NULL) {
    right = left = addrInfo(ip, port);
  } else {
    addrInfo oldRight = right;
    right = addrInfo(ip, port);
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "clonepeer:" << totalPeers << ":" << totalContent << ":" << selfInfo.ip << ":" << selfInfo.port << ":" << oldRight.ip << ":" << oldRight.port;
    send_to_socket(rightSockfd, ss.str().c_str());

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_from_socket(rightSockfd, buf); // TODO: receive success

    disconnect_from_server(rightSockfd);
  }
}

void clonePeer(int _totalPeers, int _totalContent, char *ipLeft, uint8_t portLeft, char *ipRight, uint8_t portRight) {
    totalPeers = _totalPeers++;
    totalContent = _totalContent;

    left = addrInfo(ipLeft, portLeft);
    right = addrInfo(ipRight, portRight);

    printf("created %d with left %d and right %d\n", selfInfo.port, left.port, right.port);

    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "connectnewpeer:" << left.ip << ":" << left.port << ":" << selfInfo.ip << ":" << selfInfo.port;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void connectNewPeer(char *ipReplace, uint8_t portReplace, char *ipNew, uint8_t portNew) {
    if (selfInfo.ip == ipNew && selfInfo.port == portNew) {
      // completed full round of circle
      return;
    }
    
    if (left.ip == ipReplace && left.port == portReplace) {
      // replace left with the new peer
      left = addrInfo(ipNew, portNew);
      printf("%d left is now %d\n", selfInfo.port, left.port);
    }
    totalPeers++;

    // forward message to item connected to its right
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "connectNewPeer:" << ipReplace << ":" << portReplace << ":" << ipNew << ":" << portNew;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void join(char *ip, uint8_t port, char *args[]) {
    printf("%s:%d trying to join %s:%s\n", ip, port, args[1], args[2]);
    int8_t sockfd = -1;
    connect_to_server(&sockfd, args[1], args[2]);

    std::stringstream ss;
    ss << "newpeer:" << ip << ":" << port;
    send_to_socket(sockfd, ss.str().c_str());

    char buf[SOCKET_TRANSFER_LIMIT];
    receive_from_socket(sockfd, buf); // TODO: receives peer list

    disconnect_from_server(sockfd);
    selfInfo = addrInfo(ip, port);
}

void init(int8_t *sockfd, socklen_t *alen, bool join_network, char *args[]) {
    char *ip = select_ip();

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    inet_aton(ip, (struct in_addr *)&server.sin_addr.s_addr);
    server.sin_port = 0;

    create_server(sockfd, &server, alen);
    printf("%s %d\n", ip, ntohs(server.sin_port));

    if (join_network) {
    printf("join network\n");
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

        int8_t connectedsock;
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
            uint8_t port = atoi(strtok(NULL, ":"));
            addPeer(ip, port);
            // add peer with ip and port to network
        } else if (strcmp(buf, "removepeer") == 0) {
            char *ip = strtok(NULL, ":");
            char *port = strtok(NULL, ":");
            (void)ip;
            (void)port;
            // remove peer with ip and port to network
        } else if (strcmp(buf, "clonepeer") == 0) {
            int totalPeers = atoi(strtok(NULL, ":"));
            int totalContent = atoi(strtok(NULL, ":"));
            char *ipLeft = strtok(NULL, ":");
            uint8_t portLeft = atoi(strtok(NULL, ":"));
            char *ipRight = strtok(NULL, ":");
            uint8_t portRight = atoi(strtok(NULL, ":"));
            clonePeer(totalPeers, totalContent, ipLeft, portLeft, ipRight, portRight);
        } else if (strcmp(buf, "connectnewpeer") == 0) {
            char *ipReplace = strtok(NULL, ":");
            uint8_t portReplace = atoi(strtok(NULL, ":"));
            char *ipNew = strtok(NULL, ":");
            uint8_t portNew = atoi(strtok(NULL, ":"));
            connectNewPeer(ipReplace, portReplace, ipNew, portNew);
        } else {
            // TODO: some sort of error
        }
        // TODO: Error cases

        disconnect_from_server(connectedsock);

        return 0;
    }
}
