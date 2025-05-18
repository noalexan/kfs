#include "readline.h"
#include "keyboard.h"
#include "printk/printk.h"
#include "vga/tty.h"

volatile bool new_cmd = false;
char          readline_buffer[256];

char *readline(void)
{
	new_cmd = false;
	while (new_cmd != true)
		;
	return readline_buffer;
}

/*
 *  Old impl
 */
// size_t readline(char *buffer, size_t max_len, const char *prompt) {
//     size_t pos = 0;

//     if (prompt)
//         printk("%s", prompt);
//     last_char = 0;
//     while (1) {
//         if (last_char == 0)
//             continue;
//         if (last_char == '\n') {
//             buffer[pos++] = last_char;
//             break;
//         }
//         else if ((last_char == '\b' || last_char == 127) && pos > 0)
//             buffer[--pos] = 0;
//         else if (last_char >= 32 && last_char < 127)
//             buffer[pos++] = last_char;
//         last_char = 0;
//         if (pos == max_len - 1)
//             break;
//     }
//     buffer[pos] = 0;
//     return pos;
// }
