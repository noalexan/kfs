#include "shell.h"
#include "tty.h"
#include <libft.h>
#include "keymap.h"

#define NEW_LINE '\n'

char shells_buffers[MAX_TTY][256];
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

void    shell_switch(int num) {
    current_shell = shells_buffers[num];
    if (ft_strlen(current_shell) == 0) {
        shell_prompt();
    }
}

void shutdown(void)
{
	outw(0x604,0x2000);
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

void shell_handle_keycode(uint32_t ascii)
{
    uint32_t len = ft_strlen(current_shell);
	if (ascii == NEW_LINE)
	{
        if (ft_strequ(current_shell, "shutdown")) {
            shutdown();
        }
        else if (ft_strequ(current_shell, "reboot"))
            printk("\nreboot dans ta gueule");
        else if (ft_strequ(current_shell, "halt"))
            printk("\nhalt dans ton front");
        printk("%c", NEW_LINE);
        shell_prompt();
	}
	else {
        current_shell[len] = ascii;
        printk("%c", ascii);
    }
}