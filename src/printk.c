#include "vga.h"
#include <stdarg.h>
#include <utils.h>

static int ft_putchar(char c)
{
	switch (c) {
	case '\n':
		g_cursor.y++;
		fallthrough;

	case '\r':
		g_cursor.x = 0;
		break;

	case '\t':
		do {
			ft_putchar(' ');
		} while (g_cursor.x % 4);
		break;

	default:
		vga_set_char(g_cursor.x++, g_cursor.y, c);
	}

	while (g_cursor.x >= VGA_SCREEN_WIDTH) {
		g_cursor.x -= VGA_SCREEN_WIDTH;
		g_cursor.y++;
	}

	while (g_cursor.y >= VGA_SCREEN_HEIGHT) {
		vga_scroll_down();
		g_cursor.y--;
	}

	vga_set_cursor_position(g_cursor.x, g_cursor.y);
	return 1;
}

static int ft_putstr(char *str)
{
	int i = -1;

	if (str == NULL)
		return (ft_putstr("(null)"));
	while (str[++i])
		ft_putchar(str[i]);
	return (i);
}

static int ft_putnbr(int nb)
{
	if (nb == -2147483648)
		return (ft_putnbr(-21474) + ft_putnbr(83648));
	if (nb < 0)
		return (ft_putchar('-') + ft_putnbr(nb * -1));
	else if (nb >= 10)
		return (ft_putnbr(nb / 10) + ft_putnbr(nb % 10));
	else
		return (ft_putchar(nb + '0'));
}

static int ft_putunsignednbr(unsigned int nb)
{
	if (nb >= 10)
		return (ft_putnbr(nb / 10) + ft_putnbr(nb % 10));
	else
		return (ft_putchar(nb + '0'));
}

static int ft_puthexa(unsigned long nb, bool upper, bool addr)
{
	char *dict = upper ? "0123456789ABCDEF" : "0123456789abcdef";

	if (addr)
		return (ft_putstr("0x") + ft_puthexa(nb, upper, 0));
	else if (nb < 16)
		return (ft_putchar(dict[nb]));
	else
		return (ft_puthexa(nb / 16, upper, 0) + ft_puthexa(nb % 16, upper, 0));
}

void printk(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	for (unsigned long i = 0; fmt[i]; i++) {
		if (fmt[i] == '%') {
			switch (fmt[++i]) {
			case 's':
				ft_putstr(va_arg(ap, char *));
				break;

			case 'i':
			case 'd':
				ft_putnbr(va_arg(ap, int));
				break;

			case 'u':
				ft_putunsignednbr(va_arg(ap, unsigned int));
				break;

			case 'x':
				ft_puthexa(va_arg(ap, unsigned int), false, false);
				break;

			case 'X':
				ft_puthexa(va_arg(ap, unsigned int), true, false);
				break;

			case 'p':
				ft_puthexa(va_arg(ap, uint32_t), false, true);
				break;

			case 'c':
				ft_putchar(va_arg(ap, uint32_t));
				break;

			default:
				break;
			}
		} else
			ft_putchar(fmt[i]);
	}
}
