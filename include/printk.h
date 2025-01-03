#pragma once

#include <types.h>

struct s_cursor {
	u8 x;
	u8 y;
};

void put_char(char c);
void printk(const char *str);
