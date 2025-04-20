#include "keyboard.h"
#include "tty.h"

void kernel_main(void)
{
	keyboard_init();
	ttys_init();

	while (true) // hang
		;
}
