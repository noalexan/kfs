#include <printk.h>
#include <vga.h>

void put_char(char c)
{
	static struct s_cursor cursor = {0, 0};

	if (cursor.x >= VGA_SCREEN_WIDTH) {
		cursor.y += cursor.x / VGA_SCREEN_WIDTH;
		cursor.x %= VGA_SCREEN_WIDTH;
	}

	for (; cursor.y >= VGA_SCREEN_HEIGTH; cursor.y--)
		scroll_down();

	vga_put_char(cursor.x++, cursor.y, c);
}

void printk(const char *str)
{
	for (int i = 0; str[i]; i++)
		put_char(str[i]);
}
