#pragma once

#include <stdint.h>

uint8_t insert_content(char *value);

char *lookup_content(uint8_t key, char *ip, char *port);

void remove_content(uint8_t key, char *ip, char *port);
