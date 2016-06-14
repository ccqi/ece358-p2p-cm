#include <sstream>
#include <stdint.h>
#include <utility>

#include "../shared/socket.h"

#include "communication.h"
#include "content.h"
#include "state.h"

uint8_t insert_content(char *value) {
    // TODO: randomize
    for (uint8_t key = 1; key < INT8_MAX; ++key) {
        if (strcmp(lookup_content(key, self.ip, self.port), "-") != 0) {
            continue;
        }

        char *cvalue = strdup(value);
        data.insert(std::make_pair(key, cvalue));
        return key;
    }

    // TODO: error case
    return 0;
}

char *lookup_content(uint8_t key, char *ip, char *port) {
    if (data.find(key) != data.end()) {
        return data.at(key);
    }

    char *response = new char[SOCKET_TRANSFER_LIMIT];

    std::stringstream ss;
    ss << "lookupcontent:" << key << ":" << ip << ":" << port;
    forward(ss.str().c_str(), response, ip, port);

    return response;
}

void remove_content(uint8_t key, char *ip, char *port) {
    if (data.find(key) != data.end()) {
        data.erase(key);
        return;
    }

    std::stringstream ss;
    ss << "removecontent:" << key << ":" << ip << ":" << port;
    forward(ss.str().c_str(), ip, port);
}
