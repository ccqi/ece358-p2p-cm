#include <stdint.h>
#include <string.h>

#include "../shared/socket.h"

#include "communication.h"

void forward(const char *message) {
    int8_t sockfd = -1;
    connect_to_server(&sockfd, right.ip, right.port);
    send_to_socket(sockfd, message);
    disconnect_from_server(sockfd);
}

void forward(const char *message, const char *origin_ip,
             const char *origin_port) {
    if (strcmp(right.ip, origin_ip) == 0 &&
        strcmp(right.port, origin_port) == 0) {
        return;
    }

    forward(message);
}
