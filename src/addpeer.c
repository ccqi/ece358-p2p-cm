#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 0 && argc != 2) {
        printf("usage: ./addpeer [<address> <port>]");
        exit(1);
    }

    // if argc == 0: add first peer; else: add new peer to specified system
    // print non-loopback ip and port where peer is created
    (void)argv;

    return 0;
}
