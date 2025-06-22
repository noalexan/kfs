#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>

void kernel_main()
{
	keyboard_init();
	ttys_init();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
