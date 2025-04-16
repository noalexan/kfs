#pragma once

#include "printk.h"

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef char  i8;
typedef short i16;
typedef int   i32;

#define true  1
#define false 0

void sleep(u32 timer_count);
u32  strlen(const char *s);
void memset(void *dst, u8 value, u32 len);
void memmove(void *dest, const void *src, u32 len);
void memswap(void *a, void *b, u32 len);
int  strsame(const char *a, const char *b);
