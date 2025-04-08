#pragma once

extern "C" {
#include "../vga.h"
}

class TTY {
private:
	vga_entry       buffer[VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH];
	struct s_cursor cursor = {0, 0};
	uint8_t         mode;

public:
	TTY();
	~TTY();

	void load();
	void save();
};

extern TTY *current_tty;

#ifndef TEST
# define TEST
#endif
