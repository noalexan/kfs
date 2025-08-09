#include "internal/x86.h"
#include <drivers/tty.h>
#include <memory/memory.h>
#include <x86.h>

__attribute__((section(".stack"))) uint8_t kernel_stack[16 * 1024] __attribute__((aligned(16)));

void x86_init(void)
{

	ttys_init();
	gdt_init();
	idt_init();
	idt_register_interrupt_handlers(14, (irqHandler)page_fault_handler);

	mb2_init();
	vmm_finalize();

	page_descriptor_init();
}
