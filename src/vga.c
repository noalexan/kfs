#include "vga.h"
#include "io.h"
#include <libft.h>

static uint8_t vga_mode = FOREGROUND_WHITE | BACKGROUND_BLACK;

void           vga_set_cursor_position(uint8_t x, uint8_t y)
{
	uint16_t pos = y * VGA_SCREEN_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, pos);
	outb(0x3D4, 0x0E);
	outb(0x3D5, pos >> 8);
}

struct s_cursor vga_get_cursor_position(void)
{
	uint16_t pos;

	outb(0x3D4, 0x0F);
	pos = inb(0x3D5);
	outb(0x3D4, 0x0E);
	pos |= (uint16_t)inb(0x3D5) << 8;

	return (struct s_cursor){pos % VGA_SCREEN_WIDTH, pos / VGA_SCREEN_WIDTH};
}

void vga_set_char(int x, int y, char c)
{
	*VGA_ENTRY(x % VGA_SCREEN_WIDTH, y % VGA_SCREEN_HEIGTH) =
	    (vga_entry){c, vga_mode};
}

void vga_scroll_down(void)
{
	for (int y = 0; y < (VGA_SCREEN_HEIGTH - 1); y++)
		ft_memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_SCREEN_WIDTH * 2);
	for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
		*VGA_ENTRY(x, VGA_SCREEN_HEIGTH - 1) = (vga_entry){0, vga_mode};
}

void vga_set_screen_mode(uint8_t mode)
{
	vga_mode = mode;
	for (int y = 0; y < VGA_SCREEN_HEIGTH; y++)
		for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
			VGA_ENTRY(x, y)->mode = vga_mode;
}

void vga_clear(void)
{
	ft_bzero(VGA_BUFFER, VGA_SCREEN_WIDTH * VGA_SCREEN_HEIGTH);
}
