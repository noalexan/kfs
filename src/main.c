#include <io.h>
#include <printk.h>
#include <vga.h>

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

u8 read_status()
{
	return inb(0x64);
}

u8 read_data()
{
	return inb(0x60);
}

void kernel_main()
{
	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	vga_update_cursor_position(0, 0);

	while (true) {
		if (read_status() & 0x01) {
			u8 scancode = read_data();

			if (false) {
				ft_puthexa(scancode, false, false);
				ft_putchar('\n');
			}

			else {
				switch (scancode) {
				case 0x39:
					ft_putchar(' ');
					break;

				case 0x31:
					ft_putchar('n');
					break;

				default:
					break;
				}
			}
		}

		for (int i = 0; i < 100000; i++)
			asm volatile("nop\n\t");
	}
}
