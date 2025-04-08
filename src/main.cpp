#include "tty/tty.hpp"

extern "C" {
#include "idt.h"
#include "io.h"
#include "printk.h"
#include <libft.h>
}

TTY        *current_tty;

static void memory_dump(uint32_t addr_start, uint32_t addr_end)
{
	uint32_t addr = addr_start;
	while (addr < addr_end) {
		if (addr % 8 == 0 || addr == addr_start)
			printk("%p:  \t", addr);
		if (*(uint8_t *)addr < 0x10)
			printk("0");
		printk("%x ", *(uint8_t *)(addr++));
		if (addr % 8 == 0)
			printk("\n");
	}
	if (addr % 8)
		printk("\n");
}

static void shutdown(void)
{
	outw(0x604, 0x2000); // Works in newer versions of QEMU
}

static void halt(void) { asm volatile("hlt\n\t"); }

static void reboot()
{
	while (inb(0x64) & 0x02)
		;
	outb(0x64, 0xFE);
	halt();
}

static void switch_tty(TTY *tty)
{
	current_tty->save();
	current_tty = tty;
	current_tty->load();
}

extern "C" void kernel_main()
{
	TTY ttys[12];
	current_tty     = ttys;

	int caps_lock   = false;
	int left_shift  = false;
	int right_shift = false;

	vga_clear();
	vga_set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLACK);
	vga_set_cursor_position(0, 0);

	memory_dump((uint32_t)idt_tab, (uint32_t)(idt_tab + 1 + 0x10));

	while (true) {
		if (inb(0x64) & 0x01) {           // read status
			uint8_t scancode = inb(0x60); // read data

			switch (scancode) {
			case 0x01:
				shutdown();
				break;

			case 0x02:
				printk((left_shift || right_shift) ? "!" : "1");
				break;

			case 0x03:
				printk((left_shift || right_shift) ? "@" : "2");
				break;

			case 0x04:
				printk((left_shift || right_shift) ? "#" : "3");
				break;

			case 0x05:
				printk((left_shift || right_shift) ? "$" : "4");
				break;

			case 0x06:
				printk((left_shift || right_shift) ? "%" : "5");
				break;

			case 0x07:
				printk((left_shift || right_shift) ? "^" : "6");
				break;

			case 0x08:
				printk((left_shift || right_shift) ? "&" : "7");
				break;

			case 0x09:
				printk((left_shift || right_shift) ? "*" : "8");
				break;

			case 0x0A:
				printk((left_shift || right_shift) ? "(" : "9");
				break;

			case 0x0B:
				printk((left_shift || right_shift) ? ")" : "0");
				break;

			case 0x0C:
				printk((left_shift || right_shift) ? "_" : "-");
				break;

			case 0x0D:
				printk((left_shift || right_shift) ? "+" : "=");
				break;

			case 0x0E:
				// backspace pressed
				reboot();
				break;

			case 0x0F:
				printk("\t");
				break;

			case 0x10:
				printk((caps_lock || left_shift || right_shift) ? "Q" : "q");
				break;

			case 0x11:
				printk((caps_lock || left_shift || right_shift) ? "W" : "w");
				break;

			case 0x12:
				printk((caps_lock || left_shift || right_shift) ? "E" : "e");
				break;

			case 0x13:
				printk((caps_lock || left_shift || right_shift) ? "R" : "r");
				break;

			case 0x14:
				printk((caps_lock || left_shift || right_shift) ? "T" : "t");
				break;

			case 0x15:
				printk((caps_lock || left_shift || right_shift) ? "Y" : "y");
				break;

			case 0x16:
				printk((caps_lock || left_shift || right_shift) ? "U" : "u");
				break;

			case 0x17:
				printk((caps_lock || left_shift || right_shift) ? "I" : "i");
				break;

			case 0x18:
				printk((caps_lock || left_shift || right_shift) ? "O" : "o");
				break;

			case 0x19:
				printk((caps_lock || left_shift || right_shift) ? "P" : "p");
				break;

			case 0x1A:
				printk((left_shift || right_shift) ? "{" : "[");
				break;

			case 0x1B:
				printk((left_shift || right_shift) ? "}" : "]");
				break;

			case 0x1C:
				printk("\n");
				break;

			case 0x1D:
				// left control pressed
				break;

			case 0x1E:
				printk((caps_lock || left_shift || right_shift) ? "A" : "a");
				break;

			case 0x1F:
				printk((caps_lock || left_shift || right_shift) ? "S" : "s");
				break;

			case 0x20:
				printk((caps_lock || left_shift || right_shift) ? "D" : "d");
				break;

			case 0x21:
				printk((caps_lock || left_shift || right_shift) ? "F" : "f");
				break;

			case 0x22:
				printk((caps_lock || left_shift || right_shift) ? "G" : "g");
				break;

			case 0x23:
				printk((caps_lock || left_shift || right_shift) ? "H" : "h");
				break;

			case 0x24:
				printk((caps_lock || left_shift || right_shift) ? "J" : "j");
				break;

			case 0x25:
				printk((caps_lock || left_shift || right_shift) ? "K" : "k");
				break;

			case 0x26:
				printk((caps_lock || left_shift || right_shift) ? "L" : "l");
				break;

			case 0x27:
				printk((left_shift || right_shift) ? ":" : ";");
				break;

			case 0x28:
				printk((left_shift || right_shift) ? "\"" : "'");
				break;

			case 0x29:
				printk((left_shift || right_shift) ? "~" : "`");
				break;

			case 0x2B:
				printk((left_shift || right_shift) ? "|" : "\\");
				break;

			case 0x2C:
				printk((caps_lock || left_shift || right_shift) ? "Z" : "z");
				break;

			case 0x2D:
				printk((caps_lock || left_shift || right_shift) ? "X" : "x");
				break;

			case 0x2E:
				printk((caps_lock || left_shift || right_shift) ? "C" : "c");
				break;

			case 0x2F:
				printk((caps_lock || left_shift || right_shift) ? "V" : "v");
				break;

			case 0x30:
				printk((caps_lock || left_shift || right_shift) ? "B" : "b");
				break;

			case 0x31:
				printk((caps_lock || left_shift || right_shift) ? "N" : "n");
				break;

			case 0x32:
				printk((caps_lock || left_shift || right_shift) ? "M" : "m");
				break;

			case 0x33:
				printk((left_shift || right_shift) ? "<" : ",");
				break;

			case 0x34:
				printk((left_shift || right_shift) ? ">" : ".");
				break;

			case 0x35:
				printk((left_shift || right_shift) ? "?" : "/");
				break;

			case 0x37:
				printk("*");
				break;

			case 0x39:
				printk(" ");
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
				vga_set_screen_mode(BACKGROUND_WHITE | FOREGROUND_GREEN);
				break;

			case 0x48:
				vga_set_screen_mode(BACKGROUND_LIGHTGREEN | FOREGROUND_RED);
				break;

			case 0x49:
				vga_set_screen_mode(BACKGROUND_RED | FOREGROUND_LIGHTPINK);
				break;

			case 0x4A:
				printk("-");
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

			case 0x4E:
				printk("+");
				break;

			case 0x4F:
				vga_set_screen_mode(BACKGROUND_LIGHTGREY | FOREGROUND_BLACK);
				break;

			case 0x50:
				vga_set_screen_mode(BACKGROUND_BLUE | FOREGROUND_BLACK);
				break;

			case 0x51:
				vga_set_screen_mode(BACKGROUND_BLUE | FOREGROUND_WHITE);
				break;

			case 0x52:
				vga_set_screen_mode(BACKGROUND_BLACK | FOREGROUND_WHITE);
				break;

			case 0x53:
				printk(".");
				break;

			default:
				break;
			}
		}
	}
}
