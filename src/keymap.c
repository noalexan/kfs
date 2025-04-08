#include "keymap.h"

TTY	*current_tty;
TTY ttys[12];

int caps_lock   = false;
int left_shift  = false;
int right_shift = false;

char shift_keycode_table[64] = {
    [0x02] = '!',
    [0x03] = '@',
    [0x04] = '#',
    [0x05] = '$',
    [0x06] = '%',
    [0x07] = '^',
    [0x08] = '&',
    [0x09] = '*',
    [0x0A] = '(',
    [0x0B] = ')',
    [0x0C] = '_',
    [0x0D] = '+',
    [0x1A] = '{',
    [0x1B] = '}',
    [0x27] = ':',
    [0x28] = '"',
    [0x29] = '~',
    [0x2B] = '|',
    [0x33] = '<',
    [0x34] = '>',
    [0x35] = '?'
};

char keycode_table[256] = {
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



uint32_t scancode_to_ascii(uint8_t keycode, uint8_t shift, uint8_t caps_lock)
{
    uint32_t base_char = keycode_table[keycode];
    if (base_char >= 'a' && base_char <= 'z') {
        return (caps_lock != shift) ? base_char - 32 : base_char;
    } else if (shift && shift_keycode_table[keycode]) {
        return shift_keycode_table[keycode];
    }
    return base_char;
}

static uint8_t read_status() { return inb(0x64); }

static uint8_t read_data() { return inb(0x60); }

static void shutdown(void)
{
	outw(0x604, 0x2000); // Works in newer versions of QEMU
}

void	handle_keyboard() 
{
	if (read_status() & 0x01) {
		uint8_t scancode = read_data();

		switch (scancode) {

		case 0x01:
			shutdown();
			break;

		case 0x0E:
			// backspace pressed
			break;

		case 0x1D:
			// left control pressed
			break;

		case 0x2A:
			// left shift pressed
			left_shift = true;
			break;

		case 0x36:
			// right shift pressed
			right_shift = true;
			break;

		case 0xAA:
			// left shift pressed
			left_shift = false;
			break;

		case 0xB6:
			// right shift pressed
			right_shift = false;
			break;

		case 0x38:
			// left alt pressed
			break;

		case 0x3B:
			// F1 pressed
			switch_tty(ttys);
			break;

		case 0x3C:
			// F2 pressed
			switch_tty(ttys + 1);
			break;

		case 0x3D:
			// F3 pressed
			switch_tty(ttys + 2);
			break;

		case 0x3E:
			// F4 pressed
			switch_tty(ttys + 3);
			break;

		case 0x3F:
			// F5 pressed
			switch_tty(ttys + 4);
			break;

		case 0x40:
			// F6 pressed
			switch_tty(ttys + 5);
			break;

		case 0x41:
			// F7 pressed
			switch_tty(ttys + 6);
			break;

		case 0x42:
			// F8 pressed
			switch_tty(ttys + 7);
			break;

		case 0x43:
			// F9 pressed
			switch_tty(ttys + 8);
			break;

		case 0x44:
			// F10 pressed
			switch_tty(ttys + 9);
			break;

		case 0x57:
			// F11 pressed
			switch_tty(ttys + 10);
			break;

		case 0x58:
			// F12 pressed
			switch_tty(ttys + 11);
			break;

		case 0x3A:
			// CapsLock pressed
			caps_lock = !caps_lock;
			break;

		case 0x45:
			// NumberLock pressed
			break;

		case 0x46:
			// ScrollLock pressed
			for (int i = 0; i < 12; i++)
				printk("%d : %c|\n", i, (ttys+i)->buffer[0].character);
			break;

		case 0x47:
			vga_set_screen_mode(BACKGROUND_WHITE | FOREGROUND_GREEN);
			break;

		case 0x48:
			vga_set_screen_mode(BACKGROUND_LIGHTGREEN | FOREGROUND_RED);
			break;

		case 0x49:
			vga_set_screen_mode(BACKGROUND_RED | FOREGROUND_LIGHTPINK);
			break;

		case 0x4B:
			vga_set_screen_mode(BACKGROUND_LIGHTGREEN | FOREGROUND_BROWN);
			break;

		case 0x4C:
			vga_set_screen_mode(BACKGROUND_YELLOW | FOREGROUND_GREEN);
			break;

		case 0x4D:
			vga_set_screen_mode(BACKGROUND_BLACK | FOREGROUND_PINK);
			break;

		case 0x4F:
			vga_set_screen_mode(BACKGROUND_LIGHTGREY | FOREGROUND_BLACK);
			break;

		case 0x50:
			vga_set_screen_mode(BACKGROUND_BLUE | FOREGROUND_BLACK);
			break;

		default:
			if (scancode < 0x73 && scancode > 0x01) {
				char ascii = scancode_to_ascii(
					scancode,
					(left_shift || right_shift),
					caps_lock
				);
				printk("%c", ascii);
			}
			break;
		}
	}
}