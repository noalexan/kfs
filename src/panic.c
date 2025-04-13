#include "panic.h"

static void print_ptr(uint32_t *ptr) { printk("  %p: 0x%x\n", ptr, *ptr); }

void print_stack_frame()
{
	uint32_t *ebp, *esp;

	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	asm volatile("mov %%esp, %0" : "=r"(esp));

	printk("\nStack trace:\n");
	print_memory_frame(esp, ebp);

	uint32_t eip = *(ebp + 1);
	printk("Return address: 0x%x\n", eip);
}

void print_memory_frame(uint32_t *start, uint32_t *end)
{
	if (start >= end) {
		printk("Error: Invalid memory frame arguments (%p >= %p)\n", start, end);
		return;
	}

	printk("Memory frame from %p to %p:\n", start, end);
	for (uint32_t *ptr = end; ptr >= start; ptr--) {
		if ((uint32_t)ptr % sizeof(uint32_t) != 0)
			continue;
		print_ptr(ptr);
	}
}

void kpanic(char *error_msg)
{
	asm volatile("cli");
	printk("\n------------------------------------\n");
	print_stack_frame();
	printk("------------------------------------\n");
	printk("PANIC: %s\n", error_msg);
	halt();
}
