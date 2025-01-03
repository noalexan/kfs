#pragma once

#include <types.h>

#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGTH 25

#define VGA_ENTRY(x, y) (vga_buffer + ((y) * VGA_SCREEN_WIDTH + x))

enum VGA_SCREEN_MODE {
	FOREGROUND_BLACK,
	FOREGROUND_BLUE,
	FOREGROUND_GREEN,
	FOREGROUND_CYAN,
	FOREGROUND_RED,
	FOREGROUND_PINK,
	FOREGROUND_BROWN,
	FOREGROUND_LIGHTGREY,
	FOREGROUND_GREY,
	FOREGROUND_PURPLE,
	FOREGROUND_LIGHTGREEN,
	FOREGROUND_LIGHTBLUE,
	FOREGROUND_LIGHTRED,
	FOREGROUND_LIGHTPINK,
	FOREGROUND_YELLOW,
	FOREGROUND_WHITE,
	BACKGROUND_BLACK = 0,
	BACKGROUND_BLUE = 1 << 4,
	BACKGROUND_GREEN = 2 << 4,
	BACKGROUND_CYAN = 3 << 4,
	BACKGROUND_RED = 4 << 4,
	BACKGROUND_PINK = 5 << 4,
	BACKGROUND_BROWN = 6 << 4,
	BACKGROUND_LIGHTGREY = 7 << 4,
	BACKGROUND_GREY = 8 << 4,
	BACKGROUND_PURPLE = 9 << 4,
	BACKGROUND_LIGHTGREEN = 10 << 4,
	BACKGROUND_LIGHTBLUE = 11 << 4,
	BACKGROUND_LIGHTRED = 12 << 4,
	BACKGROUND_LIGHTPINK = 13 << 4,
	BACKGROUND_YELLOW = 14 << 4,
	BACKGROUND_WHITE = 15 << 4
};

typedef struct __attribute__((packed)) s_vga_entry {
	u8 character;
	u8 mode;
} vga_entry;

extern vga_entry *const vga_buffer;
extern u8 vga_screen_mode;

void vga_update_cursor(int x, int y);
void vga_enable_cursor(u8 cursor_start, u8 cursor_end);
void vga_put_char(int x, int y, char c);
void scroll_down(void);
void set_screen_mode(u8 mode);
void clear_screen(void);
