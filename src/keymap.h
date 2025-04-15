#pragma once

#include "io.h"
#include "printk.h"
#include "tty.h"
#include "types.h"
#include "vga.h"

typedef bool (*key_handler_t)(uint8_t);

extern TTY *current_tty;

void     handle_keyboard(void);
bool     special_key(uint8_t keycode);
bool     switch_tty_key(uint8_t keycode);
bool     switch_color_key(uint8_t keycode);
bool     printable_key(uint8_t keycode);
uint32_t scancode_to_ascii(uint8_t keycode);