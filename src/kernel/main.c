#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>

void kernel_main()
{
	keyboard_init();
	// uint16_t *high = 0xf0000000;
	// *high = 0;
	while (true) // hang
		__asm__ volatile("hlt");
	;
}
