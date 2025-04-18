#include "io.h"

void shutdown(void)
{
	// Works in newer versions of QEMU
	outw(0x604, 0x2000);
}

void halt(void) { __asm__ volatile("hlt\n\t"); }

void reboot(void)
{
	while (inb(0x64) & 0x02)
		;
	outb(0x64, 0xFE);
	halt();
}
