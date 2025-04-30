#pragma once

#include "acpi.h"
#include "printk.h"
#include <types.h>

#define kpanic(msg)                                                                                \
	do {                                                                                           \
		vga_set_screen_mode(VGA_COLOR(VGA_COLOR_RED, VGA_COLOR_WHITE));                            \
		vga_disable_cursor();                                                                      \
		printk("\n------------------------------------\n");                                        \
		print_stack_frame();                                                                       \
		printk("------------------------------------\n");                                          \
		printk("PANIC in %s:%d: %s\n", __FILE__, __LINE__, msg);                                   \
		halt();                                                                                    \
	} while (0)

void print_stack_frame();
void memory_dump(uint32_t addr_start, uint32_t addr_end);
void save_stack(void);
