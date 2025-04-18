#include "tty.h"
#include "printk.h"
#include <libft.h>

void tty_init(TTY *tty)
{
	tty->cursor.x = 0;
	tty->cursor.y = 0;
	tty->mode     = VGA_COLOR(VGA_COLOR_BLACK, VGA_COLOR_WHITE);
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		tty->buffer[i].character = 0;
		tty->buffer[i].mode      = tty->mode;
	}
}

void ttys_init(void)
{
	for (unsigned long i = 0; i < MAX_TTY; i++)
		tty_init(ttys + i);
	current_tty = ttys;
}

void tty_load(TTY *tty)
{
	ft_memcpy(VGA_BUFFER, tty->buffer, sizeof(tty->buffer));
	vga_set_cursor_position(tty->cursor.x, tty->cursor.y);
}

void tty_save(TTY *tty)
{
	ft_memcpy(tty->buffer, VGA_BUFFER, sizeof(tty->buffer));
	tty->cursor = g_cursor;
}

void tty_switch(TTY *tty)
{
	tty_save(current_tty);
	current_tty = tty;
	tty_load(tty);
}
