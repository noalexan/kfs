#include "tty.h"
#include "acpi.h"
#include "printk.h"
#include <libft.h>

#define NEW_LINE    '\n'
#define MAX_CMD_LEN 253

bool ft_strequ(char *s1, char *s2)
{
	if (ft_strlen(s1) != ft_strlen(s2))
		return false;
	for (int i = 0; s1[i]; i++) {
		if (s1[i] != s2[i])
			return false;
	}
	return true;
}

void tty_prompt(void)
{
	ft_bzero(current_tty->cli, 256);
	printk("$> ");
	ft_memcpy(current_tty->cli, "$> ", 3);
}

void print_help(void)
{
	printk("\nUsage: [command] [options]\n\n");
	printk("Available commands:\n");
	printk("  shutdown   Shut down the system\n");
	printk("  poweroff   Shut down the system\n");
	printk("  halt       Halt the system\n");
	printk("  reboot     Reboot the system\n");
	printk("  clear      Clear display\n");
	printk("  help       Display this help message\n\n");
}

void tty_cli_hanlde_nl(void)
{
	char    *cmd     = &current_tty->cli[3];
	uint32_t cmd_len = ft_strlen(cmd);

	if (ft_strequ(cmd, "shutdown") || ft_strequ(cmd, "poweroff")) {
		shutdown();
	} else if (ft_strequ(cmd, "reboot"))
		reboot();
	else if (ft_strequ(cmd, "halt"))
		halt();
	else if (ft_strequ(cmd, "help"))
		print_help();
	else if (ft_strequ(cmd, "clear")) {
		vga_setup_default_screen(current_tty->mode);
		return;
	} else if (cmd_len)
		printk("\nk1tOS: command not found: %s", cmd);
	printk("%c", NEW_LINE);
}

void tty_cli_handle_ascii(char ascii)
{
	char    *cmd     = &current_tty->cli[3];
	uint32_t cmd_len = ft_strlen(cmd);
	if (cmd_len < MAX_CMD_LEN)
		cmd[cmd_len] = ascii;
	printk("%c", ascii);
}

void init_tty(TTY *tty)
{
	tty->cursor.x = 0;
	tty->cursor.y = 0;
	tty->mode     = DEFAULT_COLORS;
	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
		tty->buffer[i].character = 0;
		tty->buffer[i].mode      = tty->mode;
	}
}

void tty_switch_color(uint8_t mode)
{
	current_tty->mode = mode;
	vga_set_screen_mode(current_tty->mode);
}

void init_ttys(void)
{
	for (unsigned long i = 0; i < MAX_TTY; i++)
		init_tty(ttys + i);
	current_tty = ttys;
	vga_setup_default_screen(current_tty->mode);
	tty_prompt();
}

void load_tty(TTY *tty)
{
	ft_memcpy(VGA_BUFFER, tty->buffer, sizeof(tty->buffer));
	vga_set_cursor_position(tty->cursor.x, tty->cursor.y);
	if (ft_strlen(current_tty->cli) == 0) {
		tty_prompt();
	}
	vga_set_screen_mode(current_tty->mode);
}

void save_tty(TTY *tty)
{
	ft_memcpy(tty->buffer, VGA_BUFFER, sizeof(tty->buffer));
	tty->cursor = g_cursor;
}

void switch_tty(TTY *tty)
{
	save_tty(current_tty);
	current_tty = tty;
	load_tty(tty);
}
