#include "keymap.h"
#include "acpi.h"

TTY *current_tty;
TTY  ttys[12];

key_handler_t key_handlers[5];

key_handler_t key_handlers[] = {special_key, switch_tty_key, switch_color_key, printable_key, NULL};

static bool caps_lock   = false;
static bool left_shift  = false;
static bool right_shift = false;

static char shift_keycode_table[64] = {
    [0x02] = '!', [0x03] = '@', [0x04] = '#', [0x05] = '$', [0x06] = '%', [0x07] = '^',
    [0x08] = '&', [0x09] = '*', [0x0A] = '(', [0x0B] = ')', [0x0C] = '_', [0x0D] = '+',
    [0x1A] = '{', [0x1B] = '}', [0x27] = ':', [0x28] = '"', [0x29] = '~', [0x2B] = '|',
    [0x33] = '<', [0x34] = '>', [0x35] = '?'};

static char keycode_table[256] = {
    // base
    [0x1E] = 'a',
    [0x30] = 'b',
    [0x2E] = 'c',
    [0x20] = 'd',
    [0x12] = 'e',
    [0x21] = 'f',
    [0x22] = 'g',
    [0x23] = 'h',
    [0x17] = 'i',
    [0x24] = 'j',
    [0x25] = 'k',
    [0x26] = 'l',
    [0x32] = 'm',
    [0x31] = 'n',
    [0x18] = 'o',
    [0x19] = 'p',
    [0x10] = 'q',
    [0x13] = 'r',
    [0x1F] = 's',
    [0x14] = 't',
    [0x16] = 'u',
    [0x2F] = 'v',
    [0x11] = 'w',
    [0x2D] = 'x',
    [0x15] = 'y',
    [0x2C] = 'z',
    // top keys
    [0x02] = '1',
    [0x03] = '2',
    [0x04] = '3',
    [0x05] = '4',
    [0x06] = '5',
    [0x07] = '6',
    [0x08] = '7',
    [0x09] = '8',
    [0x0A] = '9',
    [0x0B] = '0',
    // keypad
    [0x52] = '0',
    [0x4F] = '1',
    [0x50] = '2',
    [0x51] = '3',
    [0x4B] = '4',
    [0x4C] = '5',
    [0x4D] = '6',
    [0x47] = '7',
    [0x48] = '8',
    [0x49] = '9',
    [0x37] = '*',
    [0x4A] = '-',
    [0x4E] = '+',
    [0x35] = '/',
    [0x1C] = '\n',
    [0x53] = '.',
    // whitespace
    [0x39] = ' ',
    [0x0F] = '\t',
    // others
    [0x0c] = '-',
    [0x0d] = '=',
    [0x1a] = '[',
    [0x1b] = ']',
    [0x27] = ';',
    [0x2b] = '\\',
    [0x33] = ',',
    [0x34] = '.',
    [0x29] = '`',
    [0x28] = '\'',
    [0x56] = '/',
    [0x73] = '-',
};

bool special_key(uint8_t keycode)
{
	switch (keycode) {
	case 0x01:
		shutdown();
		break; // echap
	case 0x1D:
		return true;
		break; // left control
	case 0x2A:
		left_shift = true;
		break; // left shift
	case 0x36:
		right_shift = true;
		break; // right shift
	case 0xAA:
		left_shift = false;
		break; // left shift
	case 0xB6:
		right_shift = false;
		break; // right shift
	case 0x38:
		return true;
		break; // left alt
	case 0x3A:
		caps_lock = !caps_lock;
		break; // CapsLock
	case 0x45:
		return true;
		break; // NumberLock
	case 0x46:
		return true;
		break; // ScrollLock
	default:
		return false;
	}
	return true;
}

bool switch_tty_key(uint8_t keycode)
{
	uint8_t ret;
	switch (keycode) {
	case 0x3B:
		ret = 0;
		break; // F1
	case 0x3C:
		ret = 1;
		break; // F2
	case 0x3D:
		ret = 2;
		break; // F3
	case 0x3E:
		ret = 3;
		break; // F4
	case 0x3F:
		ret = 4;
		break; // F5
	case 0x40:
		ret = 5;
		break; // F6
	case 0x41:
		ret = 6;
		break; // F7
	case 0x42:
		ret = 7;
		break; // F8
	case 0x43:
		ret = 8;
		break; // F9
	case 0x44:
		ret = 9;
		break; // F10
	case 0x57:
		ret = 10;
		break; // F11
	case 0x58:
		ret = 11;
		break; // F12
	default:
		return false;
	}
	switch_tty(ttys + ret);
	shell_switch(ret);
	return true;
}

bool switch_color_key(uint8_t keycode)
{
	uint8_t ret;
	switch (keycode) {
	case 0x47:
		ret = VGA_COLOR(VGA_COLOR_WHITE, VGA_COLOR_GREEN);
		break;
	case 0x48:
		ret = VGA_COLOR(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_RED);
		break;
	case 0x49:
		ret = VGA_COLOR(VGA_COLOR_RED, VGA_COLOR_LIGHT_MAGENTA);
		break;
	case 0x4B:
		ret = VGA_COLOR(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BROWN);
		break;
	case 0x4C:
		ret = VGA_COLOR(VGA_COLOR_YELLOW, VGA_COLOR_GREEN);
		break;
	case 0x4D:
		ret = VGA_COLOR(VGA_COLOR_BLACK, VGA_COLOR_MAGENTA);
		break;
	case 0x50:
		ret = VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
		break;
	default:
		return false;
	}
	vga_set_screen_mode(ret);
	return true;
}

bool printable_key(uint8_t keycode)
{
	if (keycode < 0x73 && keycode > 0x01 || keycode == 0x0E) {
		if (keycode == 0x0E)
			return true;
		char ascii = scancode_to_ascii(keycode, (left_shift || right_shift), caps_lock);
		shell_handle_keycode(ascii);
		return true;
	}
	return false;
}

static uint32_t scancode_to_ascii(uint8_t keycode, bool shift, bool caps_lock)
{
	uint32_t base_char = keycode_table[keycode];
	if (base_char >= 'a' && base_char <= 'z') {
		return (caps_lock != shift) ? base_char - 32 : base_char;
	} else if (shift && shift_keycode_table[keycode]) {
		return shift_keycode_table[keycode];
	}
	return base_char;
}

void handle_keyboard(void)
{
	if (inb(0x64) & 0x01) {          // read status
		uint8_t keycode = inb(0x60); // read data
		for (int i = 0; key_handlers[i]; i++) {
			key_handler_t f = key_handlers[i];
			if (f(keycode))
				break;
		}
	}
}
