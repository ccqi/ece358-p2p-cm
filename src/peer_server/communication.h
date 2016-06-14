#pragma once

void forward(const char *message);
void forward(const char *message, char *response);
void forward(const char *message, const char *origin_ip,
             const char *origin_port);
void forward(const char *message, char *response, const char *origin_ip,
             const char *origin_port);

char *select_ip();
