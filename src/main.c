#include "gdt.h"
#include "keymap.h"
#include "printk.h"
#include "shell.h"
#include "tty.h"
#include "vga.h"

static void debug_printing()
{
	printk("Hello, World 42!\n");
	printk("GDT base: 0x%x, limit: 0x%x, entries 0x%p, segdesc size :%x\n", gdtr.base, gdtr.limit,
	       (void *)gdt_entries, sizeof(segment_descriptor_t));
}

static void init_builtin(void)
{
	init_ttys();
	shell_init();
	debug_printing();
	shell_prompt();
}

void kernel_main(void)
{
	init_builtin();
	while (true) {
		handle_keyboard();
	}
}
