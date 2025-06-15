#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "mb2_info.h"
#include "tty.h"
#include <memory.h>

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	mb2_init(magic, mbi_addr);
	page_descriptor_init();
	keyboard_init();
	ttys_init();
	// boot_allocator_printer();
	buddy_init();
	// debug_buddy();

	while (true) // hang
		__asm__ volatile("hlt");
	;
}