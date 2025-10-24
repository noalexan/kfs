#include "vga.h"
#include "drivers/keyboard.h"

#define KERNEL_BANNER                                                                              \
	"  /$$   /$$   /$$   /$$               /$$   /$$\n"                                            \
	" | $$  /$$/ /$$$$  | $$              | $$  / $$\n"                                            \
	" | $$ /$$/ |_  $$ /$$$$$$    /$$$$$$ |  $$/ $$/\n"                                            \
	" | $$$$$/    | $$|_  $$_/   /$$__  $$ \\  $$$$/ \n"                                           \
	" | $$  $$    | $$  | $$    | $$  \\ $$  >$$  $$ \n"                                           \
	" | $$\\  $$   | $$  | $$ /$$| $$  | $$ /$$/\\  $$\n"                                          \
	" | $$ \\  $$ /$$$$$$|  $$$$/|  $$$$$$/| $$  \\ $$\n"                                          \
	" |__/  \\__/|______/ \\___/   \\______/ |__/  |__/\n"

static uint8_t  vga_mode = VGA_DEFAULT_MODE;

static int ft_putchar(char c)
{
	switch (c) {
	case '\n':
		current_tty->cursor.y++;
		fallthrough;

	case '\r':
		current_tty->cursor.x = 0;
		break;

	case '\t':
		do {
			ft_putchar(' ');
		} while (current_tty->cursor.x % 4);
		break;

	default:
		vga_set_char(current_tty->cursor.x++, current_tty->cursor.y, c);
	}

	while (current_tty->cursor.x >= VGA_WIDTH) {
		current_tty->cursor.x -= VGA_WIDTH;
		current_tty->cursor.y++;
	}

	while (current_tty->cursor.y >= VGA_HEIGHT) {
		vga_scroll_down();
		current_tty->cursor.y--;
	}

	vga_set_cursor_position(current_tty->cursor.x, current_tty->cursor.y);
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

static int ft_putunsignednbr(uint32_t nb)
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

// Internal APIs
//////////////////////////////////////////
// External APIs

uint8_t vga_get_mode(void) { return vga_mode; }

void vga_setup_default_screen()
{
	vga_clear();
	vga_enable_cursor(14, 15);
	vga_set_cursor_position(0, 1);

	vga_printf(KERNEL_BANNER "\n" TTY_PROMPT);
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void vga_disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void vga_set_cursor_position(uint8_t x, uint8_t y)
{
	current_tty->cursor     = (struct s_cursor){x, y};
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, pos);
	outb(0x3D4, 0x0E);
	outb(0x3D5, pos >> 8);
}

void vga_set_char(int x, int y, char c)
{
	*VGA_ENTRY(x % VGA_WIDTH, y % VGA_HEIGHT) = (vga_entry){c, vga_mode};
}

void vga_scroll_down(void)
{
	for (int y = 0; y < (VGA_HEIGHT - 1); y++)
		ft_memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_WIDTH * 2);
	for (int x = 0; x < VGA_WIDTH; x++)
		*VGA_ENTRY(x, VGA_HEIGHT - 1) = (vga_entry){0, vga_mode};
}

void vga_set_screen_mode(enum vga_color mode)
{
	vga_mode = mode;
	for (int y = 0; y < VGA_HEIGHT; y++)
		for (int x = 0; x < VGA_WIDTH; x++)
			VGA_ENTRY(x, y)->mode = vga_mode;
}

void vga_set_mode(enum vga_color mode) { vga_mode = mode; }

void vga_clear(void)
{
	for (int y = 0; y < VGA_HEIGHT; y++)
		for (int x = 0; x < VGA_WIDTH; x++)
			*VGA_ENTRY(x, y) = (vga_entry){.character = 0x00, .mode = vga_mode};
}

void vga_printf(const char *fmt, ...)
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
				ft_putunsignednbr(va_arg(ap, uint32_t));
				break;

			case 'x':
				ft_puthexa(va_arg(ap, uint32_t), false, false);
				break;

			case 'X':
				ft_puthexa(va_arg(ap, uint32_t), true, false);
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
