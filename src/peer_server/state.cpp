#include <stdint.h>
#include <unordered_map>

#include "state.h"

addr_info self, left, right;

int total_peers;
int total_content;

std::unordered_map<uint8_t, char *> data;

int ceiling() {
    return (total_content + total_peers - 1) / total_peers;
}

int floor() {
    return total_content / total_peers;
}
