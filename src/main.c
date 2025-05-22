#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "mb2_info.h"
#include "vga/tty.h"

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	keyboard_init();
	ttys_init();
	mb2_init(magic, mbi_addr);

	while (true) // hang
		__asm__ volatile("hlt");
	;
}