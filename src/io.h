#pragma once

#include <types.h>

void outw(u16 port, u16 value);

void outb(u16 port, u8 byte);
u8   inb(u16 port);
