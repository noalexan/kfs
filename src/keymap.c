#include "keymap.h"
#include "acpi.h"

TTY *current_tty;
TTY  ttys[12];

// key_handler_t key_handlers[] = {special_key, switch_tty_key, switch_color_key, printable_key,
// NULL};

// static bool caps_lock   = false;
// static bool left_shift  = false;
// static bool right_shift = false;

// #define BACKSPACE 0x0E
// #define NEW_LINE  0x1C

// bool special_key(uint8_t keycode)
// {
// 	switch (keycode) {
// 	case 0x01:
// 		shutdown();
// 		break; // echap
// 	case 0x1D:
// 		return true;
// 		break; // left control
// 	case 0x2A:
// 		left_shift = true;
// 		break; // left shift
// 	case 0x36:
// 		right_shift = true;
// 		break; // right shift
// 	case 0xAA:
// 		left_shift = false;
// 		break; // left shift
// 	case 0xB6:
// 		right_shift = false;
// 		break; // right shift
// 	case 0x38:
// 		return true;
// 		break; // left alt
// 	case 0x3A:
// 		caps_lock = !caps_lock;
// 		break; // CapsLock
// 	case 0x45:
// 		return true;
// 		break; // NumberLock
// 	case 0x46:
// 		return true;
// 		break; // ScrollLock
// 	default:
// 		return false;
// 	}
// 	return true;
// }

// bool switch_tty_key(uint8_t keycode)
// {
// 	uint8_t ret;

// 	if (keycode >= 0x3B && keycode <= 0x44) // f1 -> f10
// 		ret = (keycode - 0x3B);
// 	else if (keycode == 0x57) // f11
// 		ret = 10;
// 	else if (keycode == 0x58) // f12
// 		ret = 11;
// 	else
// 		return false;
// 	switch_tty(ttys + ret);
// 	return true;
// }

// bool switch_color_key(uint8_t keycode)
// {
// 	uint8_t ret;
// 	switch (keycode) {
// 	case 0x47:
// 		ret = VGA_COLOR(VGA_COLOR_WHITE, VGA_COLOR_GREEN);
// 		break;
// 	case 0x48:
// 		ret = VGA_COLOR(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_RED);
// 		break;
// 	case 0x49:
// 		ret = VGA_COLOR(VGA_COLOR_RED, VGA_COLOR_LIGHT_MAGENTA);
// 		break;
// 	case 0x4B:
// 		ret = VGA_COLOR(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BROWN);
// 		break;
// 	case 0x4C:
// 		ret = VGA_COLOR(VGA_COLOR_YELLOW, VGA_COLOR_GREEN);
// 		break;
// 	case 0x4D:
// 		ret = VGA_COLOR(VGA_COLOR_BLACK, VGA_COLOR_MAGENTA);
// 		break;
// 	case 0x50:
// 		ret = VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_BLACK);
// 		break;
// 	default:
// 		return false;
// 	}
// 	tty_switch_color(ret);
// 	return true;
// }

// bool printable_key(uint8_t keycode)
// {
// 	if (keycode == NEW_LINE) {
// 		tty_cli_hanlde_nl();
// 		tty_prompt();
// 	} else if (keycode == BACKSPACE) {
// 		(void)keycode;
// 	} else {
// 		char ascii = scancode_to_ascii(keycode);
// 		if (!ascii)
// 			return false;
// 		tty_cli_handle_ascii(ascii);
// 	}
// 	return true;
// }

// uint32_t scancode_to_ascii(uint8_t keycode)
// {
// 	bool     shift     = (left_shift || right_shift);
// 	uint32_t base_char = printable_table[keycode];
// 	if (base_char >= 'a' && base_char <= 'z') {
// 		return (caps_lock != shift) ? base_char - 32 : base_char;
// 	} else if (shift && printable_shifted_table[keycode]) {
// 		return printable_shifted_table[keycode];
// 	}
// 	return base_char;
// }

// void handle_keyboard(void)
// {
// 	if (inb(0x64) & 0x01) {          // read status
// 		uint8_t keycode = inb(0x60); // read data
// 		for (int i = 0; key_handlers[i]; i++) {
// 			key_handler_t f = key_handlers[i];
// 			if (f(keycode))
// 				break;
// 		}
// 	}
// }
