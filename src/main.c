#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "keymap.h"
#include "printk.h"
#include "tty.h"
#include "vga.h"

// static void debug_printing()
// {
// 	printk("Hello, World 42!\n");
// 	printk("GDT base: 0x%x, limit: 0x%x, entries 0x%p, segdesc size :%x\n", gdtr.base, gdtr.limit,
// 	       (void *)gdt_entries, sizeof(segment_descriptor_t));
// }
void generate_invalid_opcode_ud2(void)
{
	__asm__ volatile("ud2" // Instruction UD2 (0x0F 0x0B)
	                 :
	                 :
	                 : "memory");
}

void generate_divide_by_zero(void)
{
	__asm__ volatile("movl $0, %%ebx\n" // Met 0 dans ebx
	                 "movl $1, %%eax\n" // Met 1 dans eax
	                 "divl %%ebx"       // Division de eax par ebx (1/0)
	                 :
	                 :
	                 : "eax", "ebx", "memory");
}

void trigger_page_fault_null(void)
{
	volatile int *ptr = (int *)0x0;
	*ptr              = 42; // Provoque un page fault (segmentation fault)
}

static void init_builtin(void)
{
	// debug_printing();
	isr_init();
	idt_init();

	// isr_register_interrupt_handler(33, handle_keyboard);
	init_ttys();
}

void kernel_main(void)
{
	init_builtin();
	trigger_page_fault_null();
	// generate_invalid_opcode_ud2();
	while (true)
		;
}
