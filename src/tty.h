#pragma once

#include "vga.h"

typedef struct TTY {
	vga_entry       buffer[VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGHT];
	struct s_cursor cursor;
	uint8_t         mode;

} TTY;

void load_tty(TTY *tty);
void save_tty(TTY *tty);
void init_tty(TTY *tty);
void init_ttys();
void switch_tty(TTY *tty);

extern TTY *current_tty;
extern TTY  ttys[12];
