#include "keymap.h"
#include "printk.h"
#include "tty.h"
#include "vga.h"
#include "shell.h"

static void init_vga()
{
	vga_clear();
	vga_set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	vga_set_cursor_position(0, 0);
}

void kernel_main()
{
	init_vga();
	printk("OH");
	init_ttys();
	shell_init();
	printk("Hello, World!\n");
	while (true) {
		handle_keyboard();
	}
}
