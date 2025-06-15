#include "internal/x86.h"

void x86_init(void)
{
	gdt_init();
	idt_init();
}
