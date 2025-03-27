#pragma once

extern "C" {
#include "../vga.h"
}

class TTY {
private:
	vga_entry       buffer[VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH];
	struct s_cursor cursor = {0, 0};
	u8              mode;

public:
	TTY();
	~TTY();

	void set_mode(u8 mode);

	size_t write(const char *buffer, size_t length);

	void load();
	void save();
};

extern TTY *current_tty;
