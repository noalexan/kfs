#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>
#include <memory/vmm.h>

void kernel_main()
{
	// paging_cleanup();
	keyboard_init();
	buddy_init();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
