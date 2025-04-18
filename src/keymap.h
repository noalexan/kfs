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

// static char printable_shifted_table[64] = {
//     [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$', [0x06] = '%', [0x07] = '^',
//     [0x08] = '&', [0x09] = '*', [0x0A] = '(', [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
//     [0x1A] = '{', [0x1B] = '}', [0x27] = ':', [0x28] = '"', [0x29] = '~', [0x2B] = '|',
//     [0x33] = '<', [0x34] = '>', [0x35] = '?'};

// static char printable_table[256] = {
//     // base
//     [0x1E] = 'a',
//     [0x30] = 'b',
//     [0x2E] = 'c',
//     [0x20] = 'd',
//     [0x12] = 'e',
//     [0x21] = 'f',
//     [0x22] = 'g',
//     [0x23] = 'h',
//     [0x17] = 'i',
//     [0x24] = 'j',
//     [0x25] = 'k',
//     [0x26] = 'l',
//     [0x32] = 'm',
//     [0x31] = 'n',
//     [0x18] = 'o',
//     [0x19] = 'p',
//     [0x10] = 'q',
//     [0x13] = 'r',
//     [0x1F] = 's',
//     [0x14] = 't',
//     [0x16] = 'u',
//     [0x2F] = 'v',
//     [0x11] = 'w',
//     [0x2D] = 'x',
//     [0x15] = 'y',
//     [0x2C] = 'z',
//     // top keys
//     [0x02] = '1',
//     [0x03] = '2',
//     [0x04] = '3',
//     [0x05] = '4',
//     [0x06] = '5',
//     [0x07] = '6',
//     [0x08] = '7',
//     [0x09] = '8',
//     [0x0A] = '9',
//     [0x0B] = '0',
//     // keypad
//     [0x52] = '0',
//     [0x4F] = '1',
//     [0x50] = '2',
//     [0x51] = '3',
//     [0x4B] = '4',
//     [0x4C] = '5',
//     [0x4D] = '6',
//     [0x47] = '7',
//     [0x48] = '8',
//     [0x49] = '9',
//     [0x37] = '*',
//     [0x4A] = '-',
//     [0x4E] = '+',
//     [0x35] = '/',
//     [0x53] = '.',
//     // whitespace
//     [0x39] = ' ',
//     [0x0F] = '\t',
//     // others
//     [0x0c] = '-',
//     [0x0d] = '=',
//     [0x1a] = '[',
//     [0x1b] = ']',
//     [0x27] = ';',
//     [0x2b] = '\\',
//     [0x33] = ',',
//     [0x34] = '.',
//     [0x29] = '`',
//     [0x28] = '\'',
//     [0x56] = '/',
//     [0x73] = '-',
// };
