#include "lib.h"

void sleep(u32 timer_count)
{
	timer_count /= 20;
	while (1) {
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
		timer_count--;
		if (timer_count <= 0)
			break;
	}
}

u32 strlen(const char *s)
{
	u32 i = 0;
	while (s[i])
		++i;
	return i;
}

void memset(void *dst, u8 value, u32 len)
{
	for (u32 i = 0; i < len; ++i) {
		((u8 *)dst)[i] = value;
	}
}

void memmove(void *dst, const void *src, u32 len)
{
	u8 *d = (u8 *)dst;
	u8 *s = (u8 *)src;
	if (d < s) {
		for (u32 i = 0; i < len; ++i) {
			d[i] = s[i];
		}
	} else {
		for (u32 i = len; 0 < i--;) {
			d[i] = s[i];
		}
	}
}

void memswap(void *a, void *b, u32 len)
{
	u8 *x = (u8 *)a;
	u8 *y = (u8 *)b;
	for (u32 i = 0; i < len; ++i) {
		u8 temp = x[i];
		x[i]    = y[i];
		y[i]    = temp;
	}
}

int strsame(const char *a, const char *b)
{
	while (*a && *b) {
		if (*a != *b)
			return 0;
		++a;
		++b;
	}
	return *a == *b;
}
