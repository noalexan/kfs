#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/boot_allocator.h>
#include <memory/buddy.h>
#include <memory/memory.h>
#include <memory/vmm.h>

void kernel_main()
{
	keyboard_init();
	buddy_init();
	// boot_allocator_print_allocations();
	// buddy_print_summary();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
