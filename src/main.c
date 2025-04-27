#include "keyboard.h"
#include "readline.h"
#include "test.h"
#include "tty.h"

void kernel_main(void)
{
	keyboard_init();
	ttys_init();

	// register_scroll_lock_on_debug();
	// test_idt_divide_by_zero();
	// test_idt_all_exceptions();
	// test_gdt_print_segment_registers();

	// printk("", 1 / 0);

	while (true) // hang
	{
		char *ret = readline();
		// printk("from readline %s\n", ret);
	}
}
