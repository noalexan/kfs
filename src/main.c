#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "mb2_info.h"
#include "memory/boot_allocator.h"
#include "vga/tty.h"

void debug(void)
{
	printk("---------------------Testing Boot Allocator---------------------\n");
	boot_allocator_printer();
	printk("-----------------------------------------------------------------\n");
	void *a = boot_alloc(64);
	void *b = boot_alloc(128);
	void *c = boot_alloc(256);

	printk("boot_alloc(64) , start = %p | end %p\n", a, a + 64);
	printk("boot_alloc(128) , start = %p | end %p\n", b, b + 128);
	printk("boot_alloc(256) , start = %p | end %p\n", c, c + 256);

	printk("-----------------------------------------------------------------\n");
	boot_allocator_printer();
	printk("-----------------------------------------------------------------\n");
}

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	keyboard_init();
	ttys_init();
	mb2_init(magic, mbi_addr);
}
