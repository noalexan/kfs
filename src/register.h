#pragma once

#include <types.h>

typedef struct {
	uint32_t ds;
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
	uint32_t interrupt, error;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed)) REGISTERS;

static inline void clean_registers(void)
{
	__asm__ volatile("xor %%eax, %%eax\n\t"
	                 "xor %%ebx, %%ebx\n\t"
	                 "xor %%ecx, %%ecx\n\t"
	                 "xor %%edx, %%edx\n\t"
	                 "xor %%esi, %%esi\n\t"
	                 "xor %%edi, %%edi\n\t"
	                 :
	                 :
	                 : "eax", "ebx", "ecx", "edx", "esi", "edi");
}
