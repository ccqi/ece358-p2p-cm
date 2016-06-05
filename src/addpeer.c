#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        printf("usage: ./addpeer [<address> <port>]\n");
        exit(1);
    }

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

    if (!fork()) {
        char *args[4];
        args[0] = "peer";
        args[1] = ip;
        args[2] = "0";
        args[3] = NULL;

        char *env[1];
        env[0] = NULL;

        if (execve("peer/peer", args, env) != 0) {
            printf("error running peer");
            exit(1);
        }
    }

    if (argc == 3) {
        // add new peer to existing system
        (void)argv;
    }

    // print non-loopback ip and port where peer is created

    return 0;
}
