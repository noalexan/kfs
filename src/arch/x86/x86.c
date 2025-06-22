#include "internal/x86.h"
#include <memory/memory.h>

void x86_init(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	mb2_init(magic, mbi_addr);
	page_descriptor_init();
	buddy_init();
}
