#pragma once

#include "acpi.h"
#include "printk.h"
#include <types.h>

void print_stack_frame();
void print_memory_frame(uint32_t *start, uint32_t *end);
void kpanic(char *error_msg);