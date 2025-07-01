#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>

void kernel_main()
{
	keyboard_init();
	ttys_init();
	// debug_buddy();
	// buddy_print(HIGHMEM_ZONE);
	boot_allocator_zones_printer();
	// boot_allocator_printer();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
