#pragma once

#include "state.h"

void forward(const char *message);
void forward(const char *message, const char *origin_ip,
             const char *origin_port);
