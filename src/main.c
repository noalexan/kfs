#include <printk.h>
#include <vga.h>
#include <io.h>

void kernel_main(void)
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	vga_update_cursor(0, 0);

	printk("Hello, World!\nfoo\n\rbar\n\tfoo\tbar");

	while (true);
}
