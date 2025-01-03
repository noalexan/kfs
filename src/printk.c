#include <printk.h>
#include <vga.h>
#include <utils.h>

void put_char(char c)
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
		do { put_char(' '); } while (cursor.x % 4);
		break;

	default:
		vga_put_char(cursor.x++, cursor.y, c);
	}

	vga_update_cursor(cursor.x, cursor.y);
}

void printk(const char *str)
{
	for (int i = 0; str[i]; i++)
		put_char(str[i]);
}
