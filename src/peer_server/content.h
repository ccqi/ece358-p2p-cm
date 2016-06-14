#pragma once

#include <stdint.h>

uint8_t insert_content(char *value);

char *read_content(uint8_t key);

void remove_content(uint8_t key, char *ip, char *port);
