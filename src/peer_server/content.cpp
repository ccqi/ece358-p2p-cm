#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "content.h"
#include "state.h"

void debug_content() {
    printf("> content is:\n");
    for (auto entry : data) {
        printf(">>> %d:%s\n", entry.first, entry.second);
    }
}

void delete_content(uint8_t key) {
    data.erase(key);
}

uint8_t insert_content(char *value) {
    char *cvalue = strdup(value);

    // TODO: find unused key
    uint8_t key = 123;
    data.insert(std::make_pair(key, cvalue));
    return key;
}

char *read_content(uint8_t key) {
    return data.at(key);
}
