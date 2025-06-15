#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <kernel/mb2_info.h>
#include <memory/memory.h>
#include <x86.h>

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
	x86_init();
	mb2_init(magic, mbi_addr);
	page_descriptor_init();
	buddy_init();
	keyboard_init();
	ttys_init();
	// boot_allocator_printer();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
