#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: ./lookupcontent <address> <port> <key>");
        exit(1);
    }

    // print content specified by key
    (void)argv;

    return 0;
}
