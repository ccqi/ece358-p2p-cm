#include <sstream>
#include <stdint.h>
#include <utility>

#include "../shared/socket.h"

#include "communication.h"
#include "content.h"
#include "state.h"

void decrement_content(char *ip, char *port) {
    --total_content;
    validate_content();

    std::stringstream ss;
    ss << "decrementcontent:" << ip << ":" << port;
    forward(ss.str().c_str(), ip, port);
}

void give_content() {
    std::pair<uint8_t, char *> content = *data.begin();
    data.erase(content.first);

    char *key = new char[sizeof(content.first) + 1];
    sprintf(key, "%d", content.first);

    std::stringstream ss;
    ss << "givecontent:" << key << ":" << content.second;
    forward(ss.str().c_str());
}

void increment_content(char *ip, char *port) {
    ++total_content;
    validate_content();

    std::stringstream ss;
    ss << "incrementcontent:" << ip << ":" << port;
    forward(ss.str().c_str(), ip, port);
}

uint8_t insert_content(char *value) {
    // TODO: randomize
    for (uint8_t key = 1; key < INT8_MAX; ++key) {
        if (strcmp(lookup_content(key, self.ip, self.port), "-") != 0) {
            continue;
        }

        char *cvalue = strdup(value);
        data.insert(std::make_pair(key, cvalue));
        increment_content(self.ip, self.port);

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

    char *ckey = new char[sizeof(key) + 1];
    sprintf(ckey, "%d", key);

    std::stringstream ss;
    ss << "lookupcontent:" << ckey << ":" << ip << ":" << port;
    forward(ss.str().c_str(), response, ip, port);

    return response;
}

void remove_content(uint8_t key, char *ip, char *port) {
    if (data.find(key) != data.end()) {
        data.erase(key);

        decrement_content(self.ip, self.port);

        return;
    }

    char *ckey = new char[sizeof(key) + 1];
    sprintf(ckey, "%d", key);

    std::stringstream ss;
    ss << "removecontent:" << ckey << ":" << ip << ":" << port;
    forward(ss.str().c_str(), ip, port);
}

void take_content() {
    char *response = new char[SOCKET_TRANSFER_LIMIT];

    std::stringstream ss;
    ss << "takecontent";
    forward(ss.str().c_str(), response);

    uint8_t key = atoi(strtok(response, ":"));
    char *value = strdup(strtok(NULL, ":"));
    data.insert(std::make_pair(key, value));
}

void validate_content() {
    if (data.size() < floor()) {
        take_content();
        validate_content();
        return;
    }

    if (data.size() > ceiling()) {
        give_content();
        validate_content();
        return;
    }
}
