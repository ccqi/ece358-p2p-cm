#pragma once

#include <stdint.h>

void decrement_content(char *ip, char *port);

void give_content();

void increment_content(char *ip, char *port);

uint8_t insert_content(char *value);

char *lookup_content(uint8_t key, char *ip, char *port);

void remove_content(uint8_t key, char *ip, char *port);

void take_content();
