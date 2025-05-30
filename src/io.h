#pragma once

#include <types.h>

static inline void outw(uint16_t port, uint16_t value)
{
	__asm__ volatile("outw %w0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline void outb(uint16_t port, uint8_t value)
{
	__asm__ volatile("outb %b0, %w1" : : "a"(value), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
	uint8_t value;

	__asm__ volatile("inb %w1, %b0" : "=a"(value) : "Nd"(port) : "memory");
	return value;
}
