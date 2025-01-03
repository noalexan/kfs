#include <vga.h>
#include <printk.h>

void kernel_main(void)
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);

	// while (true) {
	// 	char str[] = "0aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	// 	for (int i = 0; i < 10; i++) {
	// 		printk(str);
	// 		str[0]++;
	// 	}
	// }

	printk("Hello, World!");

	while (true);
}
