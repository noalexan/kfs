#include "panic.h"

static uint8_t stack_snapshot[4096];

void save_stack(void)
{
	uint32_t *ebp, *esp;
	__asm__ volatile("mov %%ebp, %0" : "=r"(ebp));
	__asm__ volatile("mov %%esp, %0" : "=r"(esp));

	for (int i = 0; i < 4096 && esp < ebp; i++)
		stack_snapshot[i] = *(uint8_t *)(esp++);
}

void print_stack_frame()
{
	uint32_t *ebp, *esp;

	__asm__ volatile("mov %%ebp, %0" : "=r"(ebp));
	__asm__ volatile("mov %%esp, %0" : "=r"(esp));

	printk("\nStack trace:\n");
	memory_dump((uint32_t)esp, (uint32_t)ebp);

	printk("ESP = %p | EBP = %p\n", esp, ebp);
	uint32_t eip = *(ebp + 1);
	printk("Return address: 0x%x\n", eip);
}

void memory_dump(uint32_t addr_start, uint32_t addr_end)
{
	uint32_t addr = addr_end;
	while (addr >= addr_start) {
		if (addr % 8 == 0 || addr == addr_start)
			printk("%p:\t", addr);
		if (*(uint8_t *)addr < 0x10)
			printk("0");
		printk("%x ", *(uint8_t *)(addr--));
		if (addr % 8 == 0)
			printk("\n");
	}
	if (addr % 8)
		printk("\n");
}
