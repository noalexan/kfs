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
}

void    shell_switch(int num) {
    current_shell = shells_buffers[num];
}

void shutdown(void)
{
	outw(0x604,0x2000);
}

void shell_handle_keycode(uint32_t ascii)
{
    uint32_t len = ft_strlen(current_shell);

	if (ascii == NEW_LINE)
	{
        if (!ft_memcmp(current_shell, "shutdown", len))
            shutdown();
        else if (!ft_memcmp(current_shell, "reboot", len))
            printk("\nreboot dans ta gueule");
        else if (!ft_memcmp(current_shell, "halt", len))
            printk("\nhalt dans ton front");
        printk("%c", NEW_LINE);
        shell_prompt();
	}
	else {
        current_shell[len] = ascii;
        printk("%c", ascii);
    }
}