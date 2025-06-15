#pragma once

#include <drivers/vga.h>
#include <types.h>

#define kpanic(msg, ...)                                                                           \
	do {                                                                                           \
		vga_clear();                                                                               \
		vga_set_screen_mode(VGA_COLOR(VGA_COLOR_RED, VGA_COLOR_WHITE));                            \
		vga_disable_cursor();                                                                      \
		vga_printf("\n------------------------------------\n");                                    \
		print_stack_frame();                                                                       \
		vga_printf("------------------------------------\n");                                      \
		vga_printf("PANIC in %s:%d\n", __FILE__, __LINE__);                                        \
		vga_printf(msg, ##__VA_ARGS__);                                                            \
		halt();                                                                                    \
	} while (0)

void __assert_fail(const char *expr, const char *file, size_t line);
void print_stack_frame(void);
