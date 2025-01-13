#include <printk.h>
#include <tty.hpp>

extern "C" {
#include <libft.h>
}

TTY::TTY()
{
	char msg[] = "Hello, World!";

	for (int i = 0; msg[i]; i++) {
		this->buffer[i].character = msg[i];
	}
}

TTY::~TTY() {}

void TTY::load() { ft_memcpy(vga_buffer, this->buffer, sizeof(this->buffer)); }

extern "C" void __stack_chk_fail_local(void) { asm volatile("hlt\n\t"); }

extern "C" void test()
{
	TTY tty;
	tty.load();
}
