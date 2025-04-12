#include "keymap.h"
#include "printk.h"
#include "shell.h"
#include "tty.h"
#include "vga.h"

static void init_vga()
{
	vga_clear();
	vga_set_screen_mode(VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_WHITE));
	vga_set_cursor_position(0, 0);
}

static void init_builtin()
{
	init_vga();
	init_ttys();
	shell_init();
}

void kernel_main()
{
	init_builtin();
	printk("Hello, World!");
	while (true) {
		handle_keyboard();
	}
}
