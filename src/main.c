#include "keymap.h"
#include "tty.h"
#include "io.h"
#include "printk.h"
#include <libft.h>
#include "vga.h"

void	vga_init()
{
	vga_clear();
	vga_set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	vga_set_cursor_position(0, 0);
}

void kernel_main()
{
	vga_init();

	printk("Hello, World!\n");
	init_ttys(12);
	while (true) {
		handle_keyboard();
	}
}
