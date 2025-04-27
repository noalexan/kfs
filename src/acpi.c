#include "io.h"

static void clean_registers(void)
{
	asm volatile("xor %%eax, %%eax\n\t"
	             "xor %%ebx, %%ebx\n\t"
	             "xor %%ecx, %%ecx\n\t"
	             "xor %%edx, %%edx\n\t"
	             "xor %%esi, %%esi\n\t"
	             "xor %%edi, %%edi\n\t"
	             :
	             :
	             : "eax", "ebx", "ecx", "edx", "esi", "edi");
}

void shutdown(void)
{
	// Works in newer versions of QEMU
	outw(0x604, 0x2000);
}

void halt(void)
{
	clean_registers();
	__asm__ volatile("hlt\n\t");
}

void reboot(void)
{
	while (inb(0x64) & 0x02)
		;
	outb(0x64, 0xFE);
	halt();
}
