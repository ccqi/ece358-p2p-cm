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

void remove_content(uint8_t key) {
    std::stringstream ss;
    ss << "removekey:" << key << ":" << self.ip << ":" << self.port;
    forward(ss.str().c_str());
}

void remove_key(uint8_t key, char *ip, char *port) {
  if (data.find(key) != data.end()) {
    data.erase(key);
    return;
  }

  if (ip == self.ip && port == self.port) {
    // throw error "Error: no such content"
    return;
  } else {
    std::stringstream ss;
    ss << "removekey:" << key << ":" << ip << ":" << port;
    forward(ss.str().c_str());
  }
}

char *read_content(uint8_t key) {
    return data.at(key);
}
