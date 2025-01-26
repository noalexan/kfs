#include <tty.hpp>

extern "C" {
#include <frames.h>
#include <io.h>
#include <libft.h>
#include <printk.h>
}

TTY *current_tty;

// void memory_dump(u32 addr_start, u32 addr_end)
// {
// 	u32 addr = addr_start;
// 	while (addr < addr_end) {
// 		if (addr % 8 == 0 || addr == addr_start)
// 			printk("%p:  \t", addr);
// 		if (*(u8 *)addr < 0x10)
// 			current_tty->writ"("0");
// 		printk("%x ", *(u8 *)(addr++));
// 		if (addr % 8 == 0)
// 			current_tty->write("\n");, 1
// 	}
// 	if (addr % 8)
// 		current_tty->write("\n"), 1;
// }

static void switch_tty(TTY *tty)
{
	current_tty->save();
	current_tty = tty;
	current_tty->load();
}

static u8 read_status() { return inb(0x64); }

static u8 read_data() { return inb(0x60); }

void spinning_mushroom()
{
	int frame = 0;

	while (true) {
		for (int y = 0; y < VGA_SCREEN_HEIGTH; y++) {
			for (int x = 0; x < VGA_SCREEN_WIDTH; x++) {
				VGA_ENTRY(x, y)->character = 223;
				VGA_ENTRY(x, y)->mode =
				    frames[frame][(y * 2 + 1) * VGA_SCREEN_WIDTH + x] << 4 |
				    frames[frame][y * 2 * VGA_SCREEN_WIDTH + x];
			}
		}

		frame++;
		frame %= sizeof(frames) / sizeof(frames[0]);

		if (read_status() & 0x01) {
			u8 scancode = read_data();
			if (scancode == 0x01)
				break;
		}

		for (int i = 0; i < 100000000; i++) {
			asm volatile("nop\n\t");
		}
	}
}

extern "C" void kernel_main()
{
	TTY ttys[12];

	char msg[] = "abcdefghijkl";
	for (size_t i = 0; i < 12; i++)
		ttys[i].write(msg + i, 1);

	current_tty = ttys;
	current_tty->load();

	int caps_lock   = false;
	int left_shift  = false;
	int right_shift = false;

	while (true) {
		if (read_status() & 0x01) {
			u8 scancode = read_data();

			switch (scancode) {
			case 0x01:
				// escape pressed
				break;

			case 0x02:
				current_tty->write((left_shift || right_shift) ? "!" : "1", 1);
				break;

			case 0x03:
				current_tty->write((left_shift || right_shift) ? "@" : "2", 1);
				break;

			case 0x04:
				current_tty->write((left_shift || right_shift) ? "#" : "3", 1);
				break;

			case 0x05:
				current_tty->write((left_shift || right_shift) ? "$" : "4", 1);
				break;

			case 0x06:
				current_tty->write((left_shift || right_shift) ? "%" : "5", 1);
				break;

			case 0x07:
				current_tty->write((left_shift || right_shift) ? "^" : "6", 1);
				break;

			case 0x08:
				current_tty->write((left_shift || right_shift) ? "&" : "7", 1);
				break;

			case 0x09:
				current_tty->write((left_shift || right_shift) ? "*" : "8", 1);
				break;

			case 0x0A:
				current_tty->write((left_shift || right_shift) ? "(" : "9", 1);
				break;

			case 0x0B:
				current_tty->write((left_shift || right_shift) ? ")" : "0", 1);
				break;

			case 0x0C:
				current_tty->write((left_shift || right_shift) ? "_" : "-", 1);
				break;

			case 0x0D:
				current_tty->write((left_shift || right_shift) ? "+" : "=", 1);
				break;

			case 0x0E:
				// backspace pressed
				break;

			case 0x0F:
				current_tty->write("\t", 1);
				break;

			case 0x10:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "Q" : "q", 1);
				break;

			case 0x11:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "W" : "w", 1);
				break;

			case 0x12:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "E" : "e", 1);
				break;

			case 0x13:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "R" : "r", 1);
				break;

			case 0x14:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "T" : "t", 1);
				break;

			case 0x15:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "Y" : "y", 1);
				break;

			case 0x16:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "U" : "u", 1);
				break;

			case 0x17:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "I" : "i", 1);
				break;

			case 0x18:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "O" : "o", 1);
				break;

			case 0x19:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "P" : "p", 1);
				break;

			case 0x1A:
				current_tty->write((left_shift || right_shift) ? "{" : "[", 1);
				break;

			case 0x1B:
				current_tty->write((left_shift || right_shift) ? "}" : "]", 1);
				break;

			case 0x1C:
				current_tty->write("\n", 1);
				break;

			case 0x1D:
				// left control pressed
				break;

			case 0x1E:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "A" : "a", 1);
				break;

			case 0x1F:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "S" : "s", 1);
				break;

			case 0x20:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "D" : "d", 1);
				break;

			case 0x21:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "F" : "f", 1);
				break;

			case 0x22:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "G" : "g", 1);
				break;

			case 0x23:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "H" : "h", 1);
				break;

			case 0x24:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "J" : "j", 1);
				break;

			case 0x25:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "K" : "k", 1);
				break;

			case 0x26:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "L" : "l", 1);
				break;

			case 0x27:
				current_tty->write((left_shift || right_shift) ? ":" : ";", 1);
				break;

			case 0x28:
				current_tty->write((left_shift || right_shift) ? "\"" : "'", 1);
				break;

			case 0x29:
				current_tty->write((left_shift || right_shift) ? "~" : "`", 1);
				break;

			case 0x2B:
				current_tty->write((left_shift || right_shift) ? "|" : "\\", 1);
				break;

			case 0x2C:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "Z" : "z", 1);
				break;

			case 0x2D:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "X" : "x", 1);
				break;

			case 0x2E:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "C" : "c", 1);
				break;

			case 0x2F:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "V" : "v", 1);
				break;

			case 0x30:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "B" : "b", 1);
				break;

			case 0x31:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "N" : "n", 1);
				break;

			case 0x32:
				current_tty->write(
				    (caps_lock || left_shift || right_shift) ? "M" : "m", 1);
				break;

			case 0x33:
				current_tty->write((left_shift || right_shift) ? "<" : ",", 1);
				break;

			case 0x34:
				current_tty->write((left_shift || right_shift) ? ">" : ".", 1);
				break;

			case 0x35:
				current_tty->write((left_shift || right_shift) ? "?" : "/", 1);
				break;

			case 0x37:
				current_tty->write("*", 1);
				break;

			case 0x39:
				current_tty->write(" ", 1);
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
				break;

				// -- Keypad --

			case 0x47:
				current_tty->set_mode(BACKGROUND_WHITE | FOREGROUND_GREEN);
				break;

			case 0x48:
				current_tty->set_mode(BACKGROUND_LIGHTGREEN | FOREGROUND_RED);
				break;

			case 0x49:
				current_tty->set_mode(BACKGROUND_RED | FOREGROUND_LIGHTPINK);
				break;

			case 0x4A:
				current_tty->write("-", 1);
				break;

			case 0x4B:
				current_tty->set_mode(BACKGROUND_LIGHTGREEN | FOREGROUND_BROWN);
				break;

			case 0x4C:
				current_tty->set_mode(BACKGROUND_YELLOW | FOREGROUND_GREEN);
				break;

			case 0x4D:
				current_tty->set_mode(BACKGROUND_BLACK | FOREGROUND_PINK);
				break;

			case 0x4E:
				current_tty->write("+", 1);
				break;

			case 0x4F:
				current_tty->set_mode(BACKGROUND_LIGHTGREY | FOREGROUND_BLACK);
				break;

			case 0x50:
				current_tty->set_mode(BACKGROUND_BLUE | FOREGROUND_BLACK);
				break;

			case 0x51:
				current_tty->set_mode(BACKGROUND_BLUE | FOREGROUND_WHITE);
				break;

			case 0x52:
				current_tty->set_mode(BACKGROUND_BLACK | FOREGROUND_WHITE);
				break;

			case 0x53:
				current_tty->write(".", 1);
				break;

			case 0xc6:
				spinning_mushroom();
				break;

			default:
				break;
			}

			current_tty->load();
		}
	}
}
