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
	buddy_init();
	// print_buddy_free_list(0);
	// debug_buddy();
	// debug();
	while (true) // hang
		__asm__ volatile("hlt");
	;
}