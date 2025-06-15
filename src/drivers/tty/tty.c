#include <libft.h>

#include "acpi.h"
#include "internal/tty.h"
#include "keyboard.h"
#include <vga.h>

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
	tty->cursor = g_cursor;
	tty->mode   = VGA_DEFAULT_MODE;

	ft_memcpy(tty->buffer, VGA_BUFFER, 2 * VGA_WIDTH * VGA_HEIGHT);
	ft_bzero(tty->cli, 256);
}

void tty_switch_color(uint8_t mode)
{
	current_tty->mode = mode;
	vga_set_screen_mode(current_tty->mode);
}

void ttys_init(void)
{
	vga_setup_default_screen();
	for (unsigned long i = 0; i < sizeof(ttys) / sizeof(TTY); i++)
		tty_init(ttys + i);
	current_tty = ttys;
}

void tty_load(TTY *tty)
{
	ft_memcpy(VGA_BUFFER, tty->buffer, sizeof(tty->buffer));
	vga_set_cursor_position(tty->cursor.x, tty->cursor.y);
	vga_set_screen_mode(current_tty->mode);
}

void tty_save(TTY *tty)
{
	ft_memcpy(tty->buffer, VGA_BUFFER, sizeof(tty->buffer));
	tty->cursor = g_cursor;
}

void tty_switch(TTY *tty)
{
	tty_save(current_tty);
	current_tty = tty;
	tty_load(tty);
}
