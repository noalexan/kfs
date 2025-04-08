#pragma once

#include "types.h"
#include "tty.h"
#include "vga.h"
#include "io.h"
#include "printk.h"

uint32_t scancode_to_ascii(uint8_t keycode, uint8_t shift_active, uint8_t caps_lock);
void handle_keyboard();
