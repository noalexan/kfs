#include "tty.h"
#include "drivers/vga.h"
#include "kernel/panic.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"

TTY *current_tty;
TTY  ttys[12];

void tty_framebuffer_set_screen_mode(enum vga_color mode)
{
	current_tty->mode = mode;
	for (unsigned long i = 0; i < VGA_WIDTH * TTY_HIST_SIZE; i++)
		current_tty->framebuffer[i].mode = mode;
}

void tty_frambuffer_switch_color(uint8_t mode)
{
	current_tty->mode = mode;
	tty_framebuffer_set_screen_mode(current_tty->mode);
}

void tty_framebuffer_clear(void)
{
	for (unsigned long i = 0; i < VGA_WIDTH * TTY_HIST_SIZE; i++)
		current_tty->framebuffer[i] = (vga_entry){.character = 0x00, .mode = current_tty->mode};
	current_tty->top_line_index = 0;
	current_tty->cursor = (struct s_cursor){0, 1};
}

void tty_frambuffer_clear_new_line(void)
{
	current_tty->top_line_index++;
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		uint8_t real_y = (uint8_t)current_tty->top_line_index + (uint8_t)current_tty->cursor.y;
		int offset = (real_y * VGA_WIDTH) + i;
		// current_tty->framebuffer[offset] = (vga_entry){.character = 0x00, .mode = current_tty->mode};
	}
}

void tty_frambuffer_write(char c) {
	if (!current_tty || !current_tty->framebuffer)
		return;
	uint8_t real_y = (uint8_t)current_tty->top_line_index + (uint8_t)current_tty->cursor.y;
    int offset = (real_y * VGA_WIDTH) + current_tty->cursor.x;
	current_tty->framebuffer[offset] = (vga_entry){c, current_tty->mode};
}


static bool ft_strequ(const char *s1, const char *s2)
{
	if (ft_strlen(s1) != ft_strlen(s2))
		return false;
	for (int i = 0; s1[i]; i++) {
		if (s1[i] != s2[i])
			return false;
	}
	return true;
}

static void print_help(void)
{
	vga_printf("\nUsage: [command] [options]\n\n"
	           "Available commands:\n"
	           "  shutdown   Shut down the system\n"
	           "  poweroff   Shut down the system\n"
	           "  halt       Halt the system\n"
	           "  reboot     Reboot the system\n"
	           "  clear      Clear display\n"
	           "  azerty     Switch layout to azerty\n"
	           "  qwerty     Switch layout to qwerty\n"
	           "  help       Display this help message\n");
}

void tty_cli_handle_nl(void)
{
	size_t cmd_len = ft_strlen(current_tty->cli);
	if (ft_strequ(current_tty->cli, "shutdown") || ft_strequ(current_tty->cli, "poweroff")) {
		shutdown();
	} else if (ft_strequ(current_tty->cli, "reboot")) {
		reboot();
	} else if (ft_strequ(current_tty->cli, "halt")) {
		halt();
	} else if (ft_strequ(current_tty->cli, "help")) {
		print_help();
	} else if (ft_strequ(current_tty->cli, "clear")) {
		vga_setup_default_screen(current_tty->mode);
		current_tty->cli[0] = 0;
		return;
	} else if (ft_strequ(current_tty->cli, "azerty")) {
		keyboard_switch_layout(AZERTY);
	} else if (ft_strequ(current_tty->cli, "qwerty")) {
		keyboard_switch_layout(QWERTY);
	} else if (cmd_len) {
		vga_printf("\nk1tOS: command not found: %s", current_tty->cli);
	}

	vga_printf("\n%s", TTY_PROMPT);
	ft_bzero(current_tty->cli, 256);
}

void tty_init(TTY *tty)
{
	tty->top_line_index        = 0;
	tty->cursor                = (struct s_cursor){0, 1};
	tty->history.status        = false;
	tty->history.watching_line = 0;
	tty->mode                  = VGA_DEFAULT_MODE;
	tty->framebuffer_size      = (VGA_WIDTH * TTY_HIST_SIZE) * sizeof(vga_entry);
	tty->framebuffer           = NULL;
	vga_enable_cursor(14, 15);
	ft_bzero(tty->cli, 256);
}

void ttys_init(void)
{
	for (unsigned long i = 0; i < sizeof(ttys) / sizeof(TTY); i++)
		tty_init(ttys + i);
	tty_switch(&ttys[0]);
}

void tty_load(TTY *tty)
{
	if (tty->framebuffer == NULL) {
		tty->framebuffer = kmalloc(tty->framebuffer_size, (GFP_KERNEL | __GFP_ZERO));
		if (tty->framebuffer == NULL)
			kpanic("No space left to init ttys.");
		tty_framebuffer_set_screen_mode(tty->mode);
		vga_setup_default_screen();
	}
	vga_refresh_screen();
}

void tty_switch(TTY *tty)
{
	current_tty = tty;
	tty_load(tty);
}
