#include "gdt.h"
#include "keymap.h"
#include "printk.h"
#include "shell.h"
#include "tty.h"
#include "vga.h"

static void init_vga(void)
{
	vga_clear();
	vga_enable_cursor(14, 15);
	vga_set_cursor_position(0, 0);
	vga_set_screen_mode(VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_WHITE));
}

static void init_builtin(void)
{
	// gdt_init();
	init_vga();
	init_ttys();
	shell_init();
}

void kernel_main(void)
{
	init_builtin();
	printk("Hello, World 42!\n");
	printk("GDT base: 0x%x, limit: 0x%x, entries 0x%p, segdesc size :%x\n", gdtr.base, gdtr.limit,
	       (void *)gdt_entries, sizeof(segment_descriptor_t));
	shell_prompt();

	while (true) {
		handle_keyboard();
	}
}
