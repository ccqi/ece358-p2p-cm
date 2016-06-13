#pragma once

#include <unordered_map>
#include <stdint.h>

extern std::unordered_map<uint8_t, char *> data;

uint8_t insert_content(char *value);

char *read_content(uint8_t key);

void delete_content(uint8_t key);

void debug_content(char *tag);