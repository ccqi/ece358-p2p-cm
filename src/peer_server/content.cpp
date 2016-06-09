#include <unordered_map>
#include <stdlib.h>

#include "content.h"

std::unordered_map<char *, char *> data;

char *insert_content(char *value) {
    // TODO: find unused key
    char *key = (char *)"123";
    data[key] = value;
    printf("%s\n", data.at(key));
    return key;
}

char *read_content(char *key) {
    // TODO: why does this not have the value that line 12 has?
    // Test case:
    // $ ./peer
    // $ ./addcontent peer_ip peer_port "test"
    // test  # printed in ./peer window
    // $ ./lookupcontent peer_ip peer_port 123
    // # error in ./peer window
    return data.at(key);
}

void delete_content(char *key) {
    data.erase(key);
}
