#pragma once

#include <drivers/vga.h>

#define MAX_TTY       12
#define TTY_HIST_SIZE 256
#define MAX_CMD_LEN   256
#define TTY_PROMPT    "$> "

typedef struct tty_history {
	bool   status;
	size_t watching_line;
} tty_history;

typedef struct TTY {
	vga_entry      *framebuffer;
	size_t          framebuffer_size;
	uint8_t         top_line_index;
	tty_history     history;
	struct s_cursor cursor;
	uint8_t         mode;
	char            cli[256];

} TTY;

void       ttys_init(void);
void       tty_init(TTY *tty);
void       tty_load(TTY *tty);
void       tty_switch(TTY *tty);
void       tty_frambuffer_switch_color(uint8_t mode);
void       tty_cli_handle_nl(void);
void       tty_framebuffer_set_screen_mode(enum vga_color mode);
void	   tty_framebuffer_clear(void);
void	   tty_frambuffer_write(char c);
void	   tty_frambuffer_clear_new_line(void);


extern TTY *current_tty;
extern TTY  ttys[MAX_TTY];
