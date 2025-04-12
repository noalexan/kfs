#include "shell.h"
#include "acpi.h"
#include "keymap.h"
#include "tty.h"
#include <libft.h>

#define NEW_LINE '\n'

char  shells_buffers[MAX_TTY][256];
char *current_shell;

void shell_init()
{
	current_shell = shells_buffers[0];
	shell_prompt();
}

void shell_prompt()
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

void print_help()
{
	printk("\nUsage: [command] [options]\n\n");
	printk("Available commands:\n");
	printk("  shutdown   Shut down the system\n");
	printk("  halt       Halt the system\n");
	printk("  reboot     Reboot the system\n");
	printk("  help       Display this help message\n\n");
}

void shell_handle_keycode(uint32_t ascii)
{
	char    *cmd = &current_shell[3];
	uint32_t len = ft_strlen(cmd);
	if (ascii == NEW_LINE) {
		if (ft_strequ(cmd, "shutdown")) {
			shutdown();
		} else if (ft_strequ(cmd, "reboot"))
			printk("\nreboot dans ta gueule");
		else if (ft_strequ(cmd, "halt"))
			printk("\nhalt dans ton front");
		else if (ft_strequ(cmd, "help"))
			print_help();
		printk("%c", NEW_LINE);
		shell_prompt();
	} else {
		cmd[len] = ascii;
		printk("%c", ascii);
	}
}