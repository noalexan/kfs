#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>
#include <memory/vmm.h>

void kernel_main()
{
	// paging_cleanup();
	// buddy_init();
	keyboard_init();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
