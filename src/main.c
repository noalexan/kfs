#include <gdt.h>
#include <io.h>
#include <printk.h>
#include <vga.h>
#include <libft.h>

#define a(a,i,b) ft_putstr(a); ft_puthexa(GDT_ENTRY(i)->b, false, true); ft_putchar('\n');

void kernel_main(void)
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	init_gdt();

	gdt_entry *ptr = GDT_ENTRY(1);
	ft_memset(ptr, 0xff, sizeof(gdt_entry));

	for (int i = 0; i <= 2; i++) {
		ft_puthexa((unsigned long)GDT_ENTRY(i), false, true);
		ft_putstr(" ->\t");
		ft_puthexa(GDT_ENTRY(i)->llimit, false, true);
		ft_putchar('\n');
		a("\t\t\t",i,lbase)
		a("\t\t\t",i,base)
		a("\t\t\t",i,access)
		a("\t\t\t",i,hlimit)
		a("\t\t\t",i,flags)
		a("\t\t\t",i,hbase)
	}

	while (true);
}
