#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: ./removecontent <address> <port> <key>\n");
        exit(1);
    }

    // remove content specified by key
    (void)argv;

    return 0;
}
