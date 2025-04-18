#include "gdt.h"
#include "idt.h"
#include "keymap.h"
#include "printk.h"
#include "shell.h"
#include "tty.h"
#include "vga.h"

static void vga_init(void)
{
	vga_set_mode(VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_WHITE));
	vga_clear();
	vga_enable_cursor(14, 15);
	vga_set_cursor_position(0, 1);
}

void kernel_main(void)
{
	vga_init();

	printk("\tInitializing gdt...\n");
	gdt_init();

	printk("\tInitializing idt...\n");
	idt_init();

	printk("\tInitializing ttys...\n");
	ttys_init();

	printk("\tInitializing shell...\n\n");
	shell_init();

	while (true) // hang
		;
}
