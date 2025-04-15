#pragma once

#include "vga.h"

#define MAX_TTY 12

typedef struct TTY {
	vga_entry       buffer[VGA_WIDTH * VGA_HEIGHT];
	struct s_cursor cursor;
	uint8_t         mode;
	char            cli[256];

} TTY;

void init_ttys(void);
void load_tty(TTY *tty);
void save_tty(TTY *tty);
void init_tty(TTY *tty);
void switch_tty(TTY *tty);
void tty_switch_color(uint8_t mode);
void tty_cli_handle_ascii(char ascii);
void tty_cli_hanlde_nl(void);

extern TTY *current_tty;
extern TTY  ttys[MAX_TTY];
