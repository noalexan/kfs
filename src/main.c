#include <gdt.h>
#include <io.h>
#include <libft.h>
#include <printk.h>
#include <vga.h>

void kernel_main(void)
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	init_gdt();

	// for (int i = 0; i <= 2; i++) {
	// 	ft_puthexa((unsigned long)GDT_ENTRY(i), false, true);
	// 	ft_putstr(" ->\t");
	// 	ft_puthexa(GDT_ENTRY(i)->llimit, false, true);
	// 	ft_putchar('\n');
	// 	a(i, lbase);
	// 	a(i, base);
	// 	a(i, access);
	// 	a(i, hlimit);
	// 	a(i, flags);
	// 	a(i, hbase);
	// }

	// Memory Dump
	u32 addr = 0x00000800;
	while (addr < 0x00000820) {
		if (addr % 8 == 0) {
			ft_puthexa(addr, false, true);
			ft_putstr(": ");
		}
		if (*(u8 *)addr < 0x10)
			ft_putchar('0');
		ft_puthexa(*(u8 *)addr, false, false);
		ft_putchar(' ');
		addr++;
		if (addr % 8 == 0)
			ft_putchar('\n');
	}

	while (true);
}
