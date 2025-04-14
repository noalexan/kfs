#include "shell.h"
#include "acpi.h"
#include "keymap.h"
#include "tty.h"
#include <libft.h>

#define NEW_LINE    '\n'
#define MAX_CMD_LEN 253

char  shells_buffers[MAX_TTY][256];
char *current_shell;

void shell_init(void)
{
	current_shell = shells_buffers[0];
	shell_prompt();
}

void shell_prompt(void)
{
	ft_bzero(current_shell, 256);
	printk("$> ");
	ft_memcpy(current_shell, "$> ", 3);
}

void shell_switch(int num)
{
	current_shell = shells_buffers[num];
	if (ft_strlen(current_shell) == 0) {
		shell_prompt();
	}
}

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

void shell_handle_keycode(uint32_t ascii)
{
	char    *cmd     = &current_shell[3];
	uint32_t cmd_len = ft_strlen(cmd);
	if (ascii == NEW_LINE) {
		if (ft_strequ(cmd, "shutdown") || ft_strequ(cmd, "poweroff")) {
			shutdown();
		} else if (ft_strequ(cmd, "reboot"))
			reboot();
		else if (ft_strequ(cmd, "halt"))
			halt();
		else if (ft_strequ(cmd, "help"))
			print_help();
		else if (ft_strequ(cmd, "clear"))
			vga_setup_default_screen(current_tty->mode);
		else if (cmd_len)
			printk("\nk1tOS: command not found: %s", current_shell + 3);
		printk("%c", NEW_LINE);
		shell_prompt();
	} else {
		if (cmd_len < MAX_CMD_LEN)
			cmd[cmd_len] = ascii;
		printk("%c", ascii);
	}
}
