#include "internal/x86.h"
#include <memory/memory.h>

__attribute__((section(".stack"))) uint8_t kernel_stack[16 * 1024] __attribute__((aligned(16)));

void x86_init(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	mb2_init(magic, mbi_addr);
	page_descriptor_init();
	pagination_init();
}
