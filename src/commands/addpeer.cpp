#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 1 && argc != 3) {
        printf("usage: ./addpeer [<address> <port>]\n");
        exit(1);
    }

    if (!fork()) {
        argv[0] = (char *)"peer";

        if (execve("peer", argv, NULL) != 0) {
            printf("error running peer");
            exit(1);
        }
    }

    return 0;
}
