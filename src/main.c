#include "keyboard.h"
#include "vga/tty.h"
// #include "memory/map.h"

void kernel_main()
{
	keyboard_init();
	ttys_init();
	// memory_init_map(magic, mbi_addr);

	while (true) // hang
		__asm__ volatile("hlt");
	;
}