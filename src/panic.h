#pragma once

#include <types.h>

#include "acpi.h"
#include "printk/printk.h"
#include "vga/vga.h"

#define kpanic(msg, ...)                                                                           \
	do {                                                                                           \
		vga_clear();                                                                               \
		vga_set_screen_mode(VGA_COLOR(VGA_COLOR_RED, VGA_COLOR_WHITE));                            \
		vga_disable_cursor();                                                                      \
		printk("\n------------------------------------\n");                                        \
		print_stack_frame();                                                                       \
		printk("------------------------------------\n");                                          \
		printk("PANIC in %s:%d\n", __FILE__, __LINE__);                                            \
		printk(msg, ##__VA_ARGS__);                                                                \
		halt();                                                                                    \
	} while (0)

void print_stack_frame();
void memory_dump(uint32_t addr_start, uint32_t addr_end);
void save_stack(void);
