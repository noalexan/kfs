#include <drivers/keyboard.h>
#include <drivers/tty.h>
#include <memory/memory.h>
#include <memory/paging.h>

void kernel_main()
{
	page_descriptors = (page_t *)PAGE_DESCRIPTORS_VADDR;
	buddy_init();
	keyboard_init();
	ttys_init();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}
