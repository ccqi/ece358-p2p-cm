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
    char *port;

    addrInfo(char *_ip, char *_port) {
        ip = _ip;
        port = _port;
    }
    addrInfo() {
    }
} selfInfo, left, right;

int totalPeers;
int totalContent;

void addPeer(char *ip, char *port) {
    printf("%s:%s adding peer %s:%s\n", selfInfo.ip, selfInfo.port, ip, port);
    printf("%s:%s right is currently %s:%s\n", selfInfo.ip, selfInfo.port,
           right.ip, right.port);
    addrInfo oldRight;
    if (right.ip == selfInfo.ip && right.port == selfInfo.port) {
        left = addrInfo(ip, port);
        oldRight = selfInfo;
    } else {
        oldRight = right;
    }
    right = addrInfo(ip, port);
    printf("%s:%s right is now %s:%s\n", selfInfo.ip, selfInfo.port, right.ip,
           right.port);
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);
    std::stringstream ss;
    ss << "clonepeer:" << totalPeers << ":" << totalContent << ":"
       << selfInfo.ip << ":" << selfInfo.port << ":" << oldRight.ip << ":"
       << oldRight.port;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
    printf("%s:%s told %s:%s to clone with right %s:%s\n", selfInfo.ip,
           selfInfo.port, right.ip, right.port, oldRight.ip, oldRight.port);
}

void clonePeer(int _totalPeers, int _totalContent, char *ipLeft, char *portLeft,
               char *ipRight, char *portRight) {
    totalPeers = _totalPeers++;
    printf("incremented peers to %d\n", totalPeers);
    totalContent = _totalContent;

    left = addrInfo(ipLeft, portLeft);
    right = addrInfo(ipRight, portRight);

    printf("created %s with left %s and right %s\n", selfInfo.port, left.port,
           right.port);

    printf("sending connectNewPeer from %s:%s to %s:%s\n", selfInfo.ip,
           selfInfo.port, right.ip, right.port);
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "connectnewpeer:" << left.ip << ":" << left.port << ":" << selfInfo.ip
       << ":" << selfInfo.port;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void connectNewPeer(char *ipReplace, char *portReplace, char *ipNew,
                    char *portNew) {
    if (selfInfo.ip == ipNew && selfInfo.port == portNew) {
        // completed full round of circle
        return;
    }

    if (left.ip == ipReplace && left.port == portReplace) {
        // replace left with the new peer
        left = addrInfo(ipNew, portNew);
        printf("%s left is now %s\n", selfInfo.port, left.port);
    }
    totalPeers++;
    printf("incremented peers to %d\n", totalPeers);
    printf("sending connectNewPeer from %s:%s to %s:%s\n", selfInfo.ip,
           selfInfo.port, right.ip, right.port);

    // forward message to item connected to its right
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "connectNewPeer:" << ipReplace << ":" << portReplace << ":" << ipNew
       << ":" << portNew;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void removeSelf(char *ip, char *port) {
    if (selfInfo.ip != ip || selfInfo.port != port) {
        // error
        return;
    }

    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "removenode:" << selfInfo.ip << ":" << selfInfo.port << ":" << left.ip
       << ":" << left.port << ":" << right.ip << ":" << right.port;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void removeNode(char *ipRemove, char *portRemove, char *ipLeft, char *portLeft,
                char *ipRight, char *portRight) {
    if (selfInfo.ip == ipRemove && selfInfo.port == portRemove) {
        // completed full round of circle, not
        return;
    }

    totalPeers--;
    printf("decremented peers to %d\n", totalPeers);

    if (right.ip == ipRemove && right.port == portRemove) {
        right = addrInfo(ipRight, portRight);
    }
    if (left.ip == ipRemove && left.port == portRemove) {
        left = addrInfo(ipLeft, portLeft);
    }

    // forward message to item connected to its right
    int8_t rightSockfd = -1;
    connect_to_server(&rightSockfd, right.ip, right.port);

    std::stringstream ss;
    ss << "removenode:" << ipRemove << ":" << portRemove;
    send_to_socket(rightSockfd, ss.str().c_str());

    disconnect_from_server(rightSockfd);
}

void join(char *ip, char *port, char *args[]) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, args[1], args[2]);
    printf("%s:%s joined %s:%s\n", ip, port, args[1], args[2]);

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
    char *port = new char[6];
    sprintf(port, "%d", server.sin_port);
    selfInfo = left = right = addrInfo(ip, port);
    totalPeers = 1;
    totalContent = 0;

    if (join_network) {
        join(ip, port, args);
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
            printf("right %s\n", right.ip);

            continue;
        }

        if (close(sockfd) < 0) {
            perror("error closing socket as child");
        }

        char buf[SOCKET_TRANSFER_LIMIT];
        receive_from_socket(connectedsock, buf);
        printf("received %s\n", buf);

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
            addPeer(ip, port);
            // add peer with ip and port to network
        } else if (strcmp(buf, "removepeer") == 0) {
            char *ip = strtok(NULL, ":");
            char *port = strtok(NULL, ":");
            removeSelf(ip, port);
            // remove peer with ip and port to network
        } else if (strcmp(buf, "removenode") == 0) {
            char *ipRemove = strtok(NULL, ":");
            char *portRemove = strtok(NULL, ":");
            char *ipLeft = strtok(NULL, ":");
            char *portLeft = strtok(NULL, ":");
            char *ipRight = strtok(NULL, ":");
            char *portRight = strtok(NULL, ":");
            removeNode(ipRemove, portRemove, ipLeft, portLeft, ipRight,
                       portRight);
            // remove peer with ip and port to network
        } else if (strcmp(buf, "clonepeer") == 0) {
            int totalPeers = atoi(strtok(NULL, ":"));
            int totalContent = atoi(strtok(NULL, ":"));
            char *ipLeft = strtok(NULL, ":");
            char *portLeft = strtok(NULL, ":");
            char *ipRight = strtok(NULL, ":");
            char *portRight = strtok(NULL, ":");
            clonePeer(totalPeers, totalContent, ipLeft, portLeft, ipRight,
                      portRight);
        } else if (strcmp(buf, "connectnewpeer") == 0) {
            char *ipReplace = strtok(NULL, ":");
            char *portReplace = strtok(NULL, ":");
            char *ipNew = strtok(NULL, ":");
            char *portNew = strtok(NULL, ":");
            connectNewPeer(ipReplace, portReplace, ipNew, portNew);
        } else {
            // TODO: some sort of error
        }
        // TODO: Error cases

        // disconnect_from_server(connectedsock);

        return 0;
    }
}
