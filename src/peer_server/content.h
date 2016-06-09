#pragma once

#include <unordered_map>

extern std::unordered_map<char *, char *> data;

char *insert_content(char *value);

char *read_content(char *key);

void delete_content(char *key);
