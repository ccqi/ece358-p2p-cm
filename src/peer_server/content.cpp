#include <unordered_map>

#include "content.h"

std::unordered_map<char *, char *> data;

char *insert_content(char *value) {
    // TODO: find unused key
    char *key = (char *)"123";
    data[key] = value;
    return key;
}

char *read_content(char *key) {
    return data.at(key);
}

void delete_content(char *key) {
    data.erase(key);
}
