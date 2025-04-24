#pragma once

#include "keyboard.h"

static keyboard_key_t azerty_layout[] = {
    // Top Key
    {'&', '1', 0x02, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'é', '2', 0x03, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'"', '3', 0x04, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'\'', '4', 0x05, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'(', '5', 0x06, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'-', '6', 0x07, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'è', '7', 0x08, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'_', '8', 0x09, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'ç', '9', 0x0A, KEY_ALPHANUMERIC, TOP_KEY, NULL},
    {'à', '0', 0x0B, KEY_ALPHANUMERIC, TOP_KEY, NULL},

    // diff
    {'a', 'A', 0x10, KEY_ALPHANUMERIC, LETTER, NULL},
    {'z', 'Z', 0x11, KEY_ALPHANUMERIC, LETTER, NULL},
    {'q', 'Q', 0x1E, KEY_ALPHANUMERIC, LETTER, NULL},
    {'m', 'M', 0x27, KEY_ALPHANUMERIC, LETTER, NULL},
    {'w', 'W', 0x2C, KEY_ALPHANUMERIC, LETTER, NULL},
    {',', '?', 0x32, KEY_ALPHANUMERIC, PUNCTUATION, NULL},
    {';', '.', 0x33, KEY_ALPHANUMERIC, PUNCTUATION, NULL},
    {':', '/', 0x34, KEY_ALPHANUMERIC, PUNCTUATION, NULL},
    {'!', '§', 0x35, KEY_ALPHANUMERIC, PUNCTUATION, NULL},
    UNDEFINED_KEY};
