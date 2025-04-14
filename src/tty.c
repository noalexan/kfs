#include "tty.h"
#include "printk.h"
#include <libft.h>

void init_tty(TTY *tty)
{
	tty->cursor.x = 0;
	tty->cursor.y = 0;
	tty->mode     = DEFAULT_COLORS;
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		tty->buffer[i].character = 0;
		tty->buffer[i].mode      = tty->mode;
	}
}

void tty_switch_color(uint8_t mode)
{
	current_tty->mode = mode;
	vga_set_screen_mode(current_tty->mode);
}

void init_ttys(void)
{
	for (unsigned long i = 0; i < MAX_TTY; i++)
		init_tty(ttys + i);
	current_tty = ttys;
	vga_setup_default_screen(current_tty->mode);
}

void load_tty(TTY *tty)
{
	ft_memcpy(VGA_BUFFER, tty->buffer, sizeof(tty->buffer));
	vga_set_cursor_position(tty->cursor.x, tty->cursor.y);
}

void save_tty(TTY *tty)
{
	ft_memcpy(tty->buffer, VGA_BUFFER, sizeof(tty->buffer));
	tty->cursor = g_cursor;
}

void switch_tty(TTY *tty)
{
	save_tty(current_tty);
	current_tty = tty;
	load_tty(tty);
}
