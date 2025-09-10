#include <drivers/keyboard.h>
#include <drivers/vga.h>
#include <memory/buddy.h>
#include <memory/kmalloc.h>
#include <memory/memory.h>
#include <memory/page.h>
#include <memory/slab.h>
#include <memory/vmalloc.h>
#include <test/mm_test.h>

void kernel_main()
{
	keyboard_init();
	buddy_init();
	slab_init();
	vmalloc_init();

	// void *test = vmalloc(127 * MiB_SIZE);
	// vga_printf("test size : %d\n", vsize(test));

	// boot_allocator_print_allocations();
	// buddy_print_summary();
	// slab_print_summary();
	// run_memory_tests();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
