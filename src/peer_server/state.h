#pragma once

#include <stdint.h>
#include <string.h>
#include <unordered_map>

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

extern addr_info self, left, right;

extern int total_peers;
extern int total_content;

extern std::unordered_map<uint8_t, char *> data;

uint16_t ceiling();
uint16_t floor();
