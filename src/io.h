#pragma once

#include <types.h>

void outw(uint16_t port, uint16_t value);

void    outb(uint16_t port, uint8_t byte);
uint8_t inb(uint16_t port);
