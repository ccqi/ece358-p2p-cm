#include <sstream>
#include <stdint.h>

#include "communication.h"
#include "content.h"
#include "state.h"

uint8_t insert_content(char *value) {
    char *cvalue = strdup(value);

    // TODO: find unused key
    uint8_t key = 123;
    data.insert(std::make_pair(key, cvalue));
    return key;
}

char *lookup_content(uint8_t key, char *ip, char *port) {
    if (data.find(key) != data.end()) {
        return data.at(key);
    }

    char *response = NULL;

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
