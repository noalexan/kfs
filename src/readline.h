#pragma once

#include <types.h>

extern volatile bool new_cmd;
extern char          readline_buffer[256];

char *readline(void);
// size_t readline(char *buffer, size_t max_len, const char *prompt);
