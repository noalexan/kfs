#pragma once

#include "io.h"
#include "register.h"

static inline void shutdown(void)
{
	// Works in newer versions of QEMU
	outw(0x604, 0x2000);
}

static inline void halt(void)
{
	clean_registers();
	__asm__ volatile("hlt");
}

static inline void reboot(void)
{
	while (inb(0x64) & 0x02)
		;
	outb(0x64, 0xFE);
	halt();
}
