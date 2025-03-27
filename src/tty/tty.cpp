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

size_t TTY::write(const char *buffer, size_t length)
{
	size_t i = 0;

	for (; i < length; i++) {
		this->buffer[this->cursor.y * VGA_SCREEN_WIDTH + cursor.x++] =
		    (vga_entry){buffer[i], mode};

		if (this->cursor.x >= VGA_SCREEN_WIDTH) {
			this->cursor.y++;
		}

		if (this->cursor.y >= VGA_SCREEN_HEIGTH) {
			ft_memcpy(this->buffer, this->buffer + VGA_SCREEN_WIDTH,
			          VGA_SCREEN_WIDTH * (VGA_SCREEN_HEIGTH - 1) * 2);
			for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
				this->buffer[VGA_SCREEN_WIDTH * (VGA_SCREEN_HEIGTH - 1) + x] =
				    (vga_entry){0, vga_screen_mode};
			cursor.y--;
		}
	}

	return i;
}

void TTY::load()
{
	ft_memcpy(vga_buffer, this->buffer, sizeof(this->buffer));
	vga_update_cursor_position(this->cursor.x, this->cursor.y);
}

void TTY::save() { ft_memcpy(this->buffer, vga_buffer, sizeof(this->buffer)); }
