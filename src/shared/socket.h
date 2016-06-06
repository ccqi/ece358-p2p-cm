#pragma once

#include <stdint.h>
#include <sys/socket.h>

#define SOCKET_TRANSFER_LIMIT 16

void create_server(int8_t *sockfd, void *server, socklen_t *alen);
