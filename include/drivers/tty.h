#pragma once

#include <drivers/vga.h>

#define MAX_TTY     12
#define MAX_CMD_LEN 256
#define TTY_PROMPT  "$> "

typedef struct TTY {
	vga_entry      *framebuffer;
	size_t          framebuffer_size;
	struct s_cursor cursor;
	uint8_t         mode;
	char            cli[256];

} TTY;

void ttys_init(void);
void tty_init(TTY *tty);
void tty_load(TTY *tty);
void tty_save(TTY *tty);
void tty_switch(TTY *tty);
void tty_switch_color(uint8_t mode);
void tty_cli_handle_nl(void);

extern TTY *current_tty;
extern TTY  ttys[MAX_TTY];
