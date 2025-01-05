#include <printk.h>
#include <utils.h>
#include <vga.h>

int ft_putchar(char c)
{
	static struct s_cursor cursor = {0, 0};

	if (cursor.x >= VGA_SCREEN_WIDTH) {
		cursor.y += cursor.x / VGA_SCREEN_WIDTH;
		cursor.x %= VGA_SCREEN_WIDTH;
	}

	for (; cursor.y >= VGA_SCREEN_HEIGTH; cursor.y--)
		scroll_down();

	switch (c) {
	case '\n':
		if (++cursor.y >= VGA_SCREEN_HEIGTH)
			scroll_down();
		fallthrough;

	case '\r':
		cursor.x = 0;
		break;

	case '\t':
		do {
			ft_putchar(' ');
		} while (cursor.x % 4);
		break;

	default:
		vga_put_char(cursor.x++, cursor.y, c);
	}

	vga_update_cursor(cursor.x, cursor.y);
	return 1;
}

int ft_putstr(char *str)
{
	int i = -1;

	if (str == NULL)
		return (ft_putstr("(null)"));
	while (str[++i])
		ft_putchar(str[i]);
	return (i);
}

int ft_putnbr(int nb)
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

int ft_putunsignednbr(unsigned int nb)
{
	if (nb >= 10)
		return (ft_putnbr(nb / 10) + ft_putnbr(nb % 10));
	else
		return (ft_putchar(nb + '0'));
}

int ft_puthexa(unsigned long nb, int upper, int addr)
{
	char *dict = upper ? "0123456789ABCDEF" : "0123456789abcdef";

	if (addr)
		return (ft_putstr("0x") + ft_puthexa(nb, upper, 0));
	else if (nb < 16)
		return (ft_putchar(dict[nb]));
	else
		return (ft_puthexa(nb / 16, upper, 0) + ft_puthexa(nb % 16, upper, 0));
}
