#ifndef ACPI_H
#define ACPI_H

#include "io.h"

inline void shutdown(void)
{
	// Works in newer versions of QEMU
	outw(0x604, 0x2000);
}

inline void halt(void) { __asm__ volatile("hlt\n\t"); }

inline void reboot(void)
{
	while (inb(0x64) & 0x02)
		;
	outb(0x64, 0xFE);
	halt();
}

#endif
