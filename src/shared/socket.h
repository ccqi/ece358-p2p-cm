#pragma once

#include <stdint.h>
#include <sys/socket.h>

#define PORT_RANGE_LO 10000
#define PORT_RANGE_HI 11000

#define SOCKET_TRANSFER_LIMIT 128

void create_server(int8_t *sockfd, void *server, socklen_t *alen);

void connect_to_server(int8_t *sockfd, char *ip, char *port);

void disconnect_from_server(int8_t sockfd);

void send_to_socket(int8_t sockfd, const char *command);

void receive_from_socket(int8_t sockfd, char *response);
