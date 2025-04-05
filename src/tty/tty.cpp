#include "tty.hpp"

extern "C" {
#include "../io.h"
#include "../printk.h"
#include <libft.h>
}

TTY::TTY() : mode(BACKGROUND_BLACK | FOREGROUND_WHITE)
{
	for (int i = 0; i < VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH; i++) {
		this->buffer[i].character = 0;
		this->buffer[i].mode      = this->mode;
	}
}

TTY::~TTY() {}

void TTY::set_mode(u8 mode)
{
	this->mode = mode;
	for (int i = 0; i < VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH; i++)
		this->buffer[i].mode = mode;
}

void TTY::load()
{
	ft_memcpy(VGA_BUFFER, this->buffer, sizeof(this->buffer));
	vga_set_cursor_position(this->cursor.x, this->cursor.y);
}

void TTY::save()
{
	ft_memcpy(this->buffer, VGA_BUFFER, sizeof(this->buffer));
	this->cursor = vga_get_cursor_position();
}
