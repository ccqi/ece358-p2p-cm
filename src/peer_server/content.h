#pragma once

#include <stdint.h>

void decrement_content(char *ip, char *port);

void increment_content(char *ip, char *port);

uint8_t insert_content(char *value);

char *lookup_content(uint8_t key, char *ip, char *port);

char *remove_content(uint8_t key, char *ip, char *port);

void validate_content();
