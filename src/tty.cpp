#include <tty.hpp>

extern "C" {
#include <printk.h>
#include <libft.h>
}

TTY::TTY()
{
	char msg[] = "Hello, World!";

	char *ptr;

	for (int i = 0; msg[i]; i++) {
		this->buffer[i].character = msg[i];
	}

	for (int i = 0; i < VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH; i++)
		this->buffer[i].mode = ((int)&ptr >> 16) % 0xff;
}

TTY::~TTY() {}

void TTY::load() { ft_memcpy(vga_buffer, this->buffer, sizeof(this->buffer)); }

extern "C" void __stack_chk_fail_local(void) { asm volatile("hlt\n\t"); }
