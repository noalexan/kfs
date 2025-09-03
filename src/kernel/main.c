#include <drivers/keyboard.h>
#include <memory/buddy.h>
#include <memory/memory.h>
#include <memory/slab.h>

void kernel_main()
{
	keyboard_init();
	buddy_init();
	slab_init();

	// boot_allocator_print_allocations();
	// buddy_print_summary();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
