#include "keyboard.h"
#include "tty.h"

void kernel_main(void)
{
	keyboard_init();
	init_ttys();

	while (true) // hang
		;
}
