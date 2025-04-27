#include "keyboard.h"
#include "test.h"
#include "tty.h"

void kernel_main(void)
{
	keyboard_init();
	ttys_init();
	// register_scroll_lock_on_debug();
	// test_idt_divide_by_zero();
	// kpanic("lol");
	// kpanic("lol");
	// test_idt_all_exceptions();
	// test_gdt_print_segment_registers();
	while (true) // hang
		;
}
