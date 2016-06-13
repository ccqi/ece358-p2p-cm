#pragma once

#include <stdint.h>

void debug_content();

void delete_content(uint8_t key);

uint8_t insert_content(char *value);

char *read_content(uint8_t key);
