#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: ./addcontent <address> <port> <value>\n");
        exit(1);
    }

    // add new content to system
    // print key for value
    (void)argv;

    return 0;
}
