#include <printk.h>
#include <vga.h>

#include <gdt.h>
#include <idt.h>

static void memory_dump(u32 addr_start, u32 addr_end)
{
	u32 addr = addr_start;
	while (addr < addr_end) {
		if (addr % 8 == 0 || addr == addr_start) {
			ft_puthexa(addr, false, true);
			ft_putstr(":  \t");
		}
		if (*(u8 *)addr < 0x10)
			ft_putchar('0');
		ft_puthexa(*(u8 *)addr, false, false);
		ft_putchar(' ');
		addr++;
		if (addr % 8 == 0)
			ft_putchar('\n');
	}
	if (addr % 8)
		ft_putchar('\n');
}

void kernel_main()
{
	// clear_screen();
	// set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);

	// ft_putstr("Hello, World!\n");

	while (true);
}
