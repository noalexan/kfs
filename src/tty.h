#pragma once

#include "vga.h"

#define MAX_TTY 12

typedef struct TTY {
	vga_entry       buffer[VGA_WIDTH * VGA_HEIGHT];
	struct s_cursor cursor;
	uint8_t         mode;
	char            cli[256];

} TTY;

void ttys_init(void);
void tty_load(TTY *tty);
void tty_save(TTY *tty);
void tty_init(TTY *tty);
void tty_switch(TTY *tty);
void tty_switch_color(uint8_t mode);
void tty_cli_handle_ascii(char ascii);
void tty_cli_handle_nl(void);
void tty_prompt(void);

extern TTY *current_tty;
extern TTY  ttys[MAX_TTY];
