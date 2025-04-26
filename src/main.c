#include "keyboard.h"
#include "test.h"
#include "tty.h"

void kernel_main(void)
{
	keyboard_init();
	ttys_init();
	// test_idt_all_exceptions();
	// test_gdt_print_segment_registers();
	while (true) // hang
		;
}
