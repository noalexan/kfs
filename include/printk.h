#pragma once

#include <types.h>

struct s_cursor {
	u8 x;
	u8 y;
};

int ft_putchar(char c);
int ft_putstr(char *str);
int ft_putnbr(int nb);
int ft_putunsignednbr(unsigned int nb);
int ft_puthexa(unsigned long nb, int upper, int addr);
