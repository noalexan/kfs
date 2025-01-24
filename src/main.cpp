#include <tty.hpp>

extern "C" {
#include <printk.h>
#include <io.h>
}

TTY *current_tty;

// void memory_dump(u32 addr_start, u32 addr_end)
// {
// 	u32 addr = addr_start;
// 	while (addr < addr_end) {
// 		if (addr % 8 == 0 || addr == addr_start)
// 			printk("%p:  \t", addr);
// 		if (*(u8 *)addr < 0x10)
// 			ft_putchar('0');
// 		printk("%x ", *(u8 *)(addr++));
// 		if (addr % 8 == 0)
// 			ft_putchar('\n');
// 	}
// 	if (addr % 8)
// 		ft_putchar('\n');
// }

static void switch_tty(TTY *tty)
{
	current_tty->save();
	current_tty = tty;
	current_tty->load();
}

static u8 read_status() { return inb(0x64); }

static u8 read_data() { return inb(0x60); }

extern "C" void kernel_main()
{
	TTY ttys[12];

	char msg[] = "abcdefghijkl";
	for (size_t i = 0; i < 12; i++)
		ttys[i].write(msg + i, 1);

	current_tty = ttys;
	current_tty->load();

	printk("Hello, World!");

	// int caps_lock   = false;
	// int left_shift  = false;
	// int right_shift = false;

	while (true) {
		if (read_status() & 0x01) {
			u8 scancode = read_data();

			switch (scancode) {
				// case 0x01:
				// 	// escape pressed
				// 	break;

				// case 0x02:
				// case 0x03:
				// case 0x04:
				// case 0x05:
				// case 0x06:
				// case 0x07:
				// case 0x08:
				// case 0x09:
				// case 0x0A:
				// 	add_char_to_buffer(0x47 + scancode);
				// 	break;

				// case 0x0B:
				// 	add_char_to_buffer('0');
				// 	break;

				// case 0x0C:
				// 	add_char_to_buffer('-');
				// 	break;

				// case 0x0D:
				// 	add_char_to_buffer('=');
				// 	break;

				// case 0x0E:
				// 	// backspace pressed
				// 	break;

				// case 0x0F:
				// 	add_char_to_buffer('\t');
				// 	break;

				// case 0x10:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'Q'
				// 	                       : 'Q' + 0x20);
				// 	break;

				// case 0x11:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'W'
				// 	                       : 'W' + 0x20);
				// 	break;

				// case 0x12:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'E'
				// 	                       : 'E' + 0x20);
				// 	break;

				// case 0x13:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'R'
				// 	                       : 'R' + 0x20);
				// 	break;

				// case 0x14:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'T'
				// 	                       : 'T' + 0x20);
				// 	break;

				// case 0x15:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'Y'
				// 	                       : 'Y' + 0x20);
				// 	break;

				// case 0x16:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'U'
				// 	                       : 'U' + 0x20);
				// 	break;

				// case 0x17:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'I'
				// 	                       : 'I' + 0x20);
				// 	break;

				// case 0x18:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'O'
				// 	                       : 'O' + 0x20);
				// 	break;

				// case 0x19:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'P'
				// 	                       : 'P' + 0x20);
				// 	break;

				// case 0x1A:
				// 	add_char_to_buffer('[');
				// 	break;

				// case 0x1B:
				// 	add_char_to_buffer(']');
				// 	break;

				// case 0x1C:
				// 	printk("\n%s", buffer);
				// 	buffer[0]    = 0;
				// 	input_length = 0;
				// 	printk("\n$> ");
				// 	break;

				// case 0x1D:
				// 	// left control pressed
				// 	break;

				// case 0x1E:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'A'
				// 	                       : 'A' + 0x20);
				// 	break;

				// case 0x1F:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'S'
				// 	                       : 'S' + 0x20);
				// 	break;

				// case 0x20:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'D'
				// 	                       : 'D' + 0x20);
				// 	break;

				// case 0x21:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'F'
				// 	                       : 'F' + 0x20);
				// 	break;

				// case 0x22:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'G'
				// 	                       : 'G' + 0x20);
				// 	break;

				// case 0x23:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'H'
				// 	                       : 'H' + 0x20);
				// 	break;

				// case 0x24:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'J'
				// 	                       : 'J' + 0x20);
				// 	break;

				// case 0x25:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'K'
				// 	                       : 'K' + 0x20);
				// 	break;

				// case 0x26:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'L'
				// 	                       : 'L' + 0x20);
				// 	break;

				// case 0x27:
				// 	add_char_to_buffer(';');
				// 	break;

				// case 0x28:
				// 	add_char_to_buffer('\'');
				// 	break;

				// case 0x29:
				// 	add_char_to_buffer('`');
				// 	break;

				// case 0x2B:
				// 	add_char_to_buffer('\\');
				// 	break;

				// case 0x2C:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'Z'
				// 	                       : 'Z' + 0x20);
				// 	break;

				// case 0x2D:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'X'
				// 	                       : 'X' + 0x20);
				// 	break;

				// case 0x2E:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'C'
				// 	                       : 'C' + 0x20);
				// 	break;

				// case 0x2F:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'V'
				// 	                       : 'V' + 0x20);
				// 	break;

				// case 0x30:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'B'
				// 	                       : 'B' + 0x20);
				// 	break;

				// case 0x31:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'N'
				// 	                       : 'N' + 0x20);
				// 	break;

				// case 0x32:
				// 	add_char_to_buffer((caps_lock || left_shift || right_shift)
				// 	                       ? 'M'
				// 	                       : 'M' + 0x20);
				// 	break;

				// case 0x33:
				// 	add_char_to_buffer(',');
				// 	break;

				// case 0x34:
				// 	add_char_to_buffer('.');
				// 	break;

				// case 0x35:
				// 	add_char_to_buffer('/');
				// 	break;

				// case 0x37:
				// 	add_char_to_buffer('*');
				// 	break;

				// case 0x39:
				// 	add_char_to_buffer(' ');
				// 	break;

				// case 0x2A:
				// 	// left shift pressed
				// 	left_shift = true;
				// 	break;

				// case 0x36:
				// 	// right shift pressed
				// 	right_shift = true;
				// 	break;

				// case 0xAA:
				// 	// left shift pressed
				// 	left_shift = false;
				// 	break;

				// case 0xB6:
				// 	// right shift pressed
				// 	right_shift = false;
				// 	break;

				// case 0x38:
				// 	// left alt pressed
				// 	break;

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

				// case 0x3A:
				// 	// CapsLock pressed
				// 	caps_lock = !caps_lock;
				// 	break;

				// case 0x45:
				// 	// NumberLock pressed
				// 	break;

				// case 0x46:
				// 	// ScrollLock pressed
				// 	break;

				// 	// -- Keypad --

				// case 0x47:
				// 	add_char_to_buffer('7');
				// 	break;

				// case 0x48:
				// 	add_char_to_buffer('8');
				// 	break;

				// case 0x49:
				// 	add_char_to_buffer('9');
				// 	break;

				// case 0x4A:
				// 	add_char_to_buffer('-');
				// 	break;

				// case 0x4B:
				// 	add_char_to_buffer('4');
				// 	break;

				// case 0x4C:
				// 	add_char_to_buffer('5');
				// 	break;

				// case 0x4D:
				// 	add_char_to_buffer('6');
				// 	break;

				// case 0x4E:
				// 	add_char_to_buffer('+');
				// 	break;

				// case 0x4F:
				// 	add_char_to_buffer('1');
				// 	break;

				// case 0x50:
				// 	add_char_to_buffer('2');
				// 	break;

				// case 0x51:
				// 	add_char_to_buffer('3');
				// 	break;

				// case 0x52:
				// 	add_char_to_buffer('0');
				// 	break;

				// case 0x53:
				// 	add_char_to_buffer('.');
				// 	break;

			default:
				break;
			}
		}
	}

	// for (int i = 0; i < 100000; i++)
	// 	asm volatile("nop\n\t");
}
