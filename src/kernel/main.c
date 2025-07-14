#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>

void kernel_main()
{
	keyboard_init();
	ttys_init();
	buddy_init();

	debug_buddy();
	// vga_printf("DMA : \n");
	// buddy_print(DMA_ZONE);
	// vga_printf("LOW : \n");
	// buddy_print(LOWMEM_ZONE);
	// vga_printf("HIGH : \n");
	// buddy_print(HIGHMEM_ZONE);

	// boot_allocator_zones_printer();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
