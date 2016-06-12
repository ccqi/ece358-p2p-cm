#pragma once

#include <string.h>

typedef struct addr_info {
    char ip[16];
    char port[6];

    addr_info(char *_ip, char *_port) {
        strncpy(ip, _ip, 16);
        strncpy(port, _port, 6);
    }

    addr_info() {
    }
} addr_info;

void add_node(char *ip, char *port);

void clone_node(int peers, int content, char *lip, char *lport, char *rip,
                char *rport);

void connect_node(char *replace_ip, char *replace_port, char *ip, char *port);

void debug_node();

void init_node(char *ip, char *port);

void remove_self(char *ip, char *port);

void remove_node(char *remove_ip, char *remove_port, char *lip, char *lport,
                 char *rip, char *rport);
