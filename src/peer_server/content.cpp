#include <unordered_map>
#include <stdlib.h>
#include <stdint.h>

#include "content.h"

std::unordered_map<uint8_t, char *> data;

uint8_t insert_content(char *value) {
    // TODO: find unused key
    uint8_t key = 123;
    data.insert(std::make_pair(key, value));
    return key;
}

char *read_content(uint8_t key) {
    return data.at(key);
}

void delete_content(uint8_t key) {
    data.erase(key);
}

void debug_content(char *tag) {
    printf("Debugging Content: %s\n", tag);
    for(auto entry : data) {
        printf("%d:%s\n", entry.first, entry.second);
    }
}