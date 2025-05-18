#include "keyboard.h"
#include "vga/tty.h"

void kernel_main(void)
{
	keyboard_init();
	ttys_init();

	while (true) // hang
		;
}
