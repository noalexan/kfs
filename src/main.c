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
	init_vga();
	init_ttys();
	shell_init();
}

void kernel_main(void)
{
	init_builtin();
	printk("Hello, World 42!\n");
	shell_prompt();
	while (true) {
		handle_keyboard();
	}
}
