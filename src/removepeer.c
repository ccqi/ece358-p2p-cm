#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: ./removepeer <address> <port>");
        exit(1);
    }

    // redistribute content on peer, remove peer
    (void)argv;

    return 0;
}
