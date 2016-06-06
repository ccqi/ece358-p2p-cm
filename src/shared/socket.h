#pragma once

#include <stdint.h>
#include <sys/socket.h>

#define SOCKET_TRANSFER_LIMIT 16

void create_server(int8_t *sockfd, void *server, socklen_t *alen);

void connect_to_server(int8_t *sockfd, char *ip, char *port);

void disconnect_from_server(uint8_t sockfd);

void send_command(int8_t sockfd, char *command);

void receive_response(int8_t sockfd, char *response);