#include <io.h>
#include <printk.h>
#include <vga.h>

void kernel_main(void)
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);

	printk("Hello, World!");

	while (true);
}
