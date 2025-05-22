#include "gdt.h"
#include "idt.h"
#include "keyboard.h"
#include "memory/mb2_info.h"
#include "memory/paging.h"
#include "vga/tty.h"

void kernel_main(uint32_t magic, uint32_t mbi_addr)
{
	gdt_init();
	idt_init();
	keyboard_init();
	ttys_init();
	mb2_init(magic, mbi_addr);
	paging_init();

	// Tests

	printk("\n");

	page_table_entry *page1 = alloc_page();
	page_table_entry *page2 = alloc_page();
	page_table_entry *page3;

	printk("page1 : %p\n"
	       "page2 : %p\n",
	       page1, page2);

	free_page(page1);
	page3 = alloc_page();

	printk("page3 : %p\n", page3);

	free_page(page2);
	free_page(page3);

	printk(TTY_PROMPT);
}
