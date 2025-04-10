#pragma once

#include "io.h"
#include "printk.h"
#include "tty.h"
#include "types.h"
#include "vga.h"

uint32_t scancode_to_ascii(uint8_t keycode, uint8_t shift_active,
                           uint8_t caps_lock);
void     handle_keyboard();
