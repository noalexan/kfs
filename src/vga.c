#include "vga.h"
#include "io.h"
#include <libft.h>

struct s_cursor g_cursor;
static uint8_t  vga_mode;

void vga_setup_default_screen(uint8_t mode)
{
	vga_set_mode(VGA_COLOR(VGA_COLOR_BLUE, VGA_COLOR_WHITE));
	vga_clear();
	vga_enable_cursor(14, 15);
	vga_set_cursor_position(0, 1);
}

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void vga_disable_cursor(void)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void vga_set_cursor_position(uint8_t x, uint8_t y)
{
	g_cursor     = (struct s_cursor){x, y};
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, pos);
	outb(0x3D4, 0x0E);
	outb(0x3D5, pos >> 8);
}

void vga_set_char(int x, int y, char c)
{
	*VGA_ENTRY(x % VGA_WIDTH, y % VGA_HEIGHT) = (vga_entry){c, vga_mode};
}

void vga_scroll_down(void)
{
	for (int y = 0; y < (VGA_HEIGHT - 1); y++)
		ft_memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_WIDTH * 2);
	for (int x = 0; x < VGA_WIDTH; x++)
		*VGA_ENTRY(x, VGA_HEIGHT - 1) = (vga_entry){0, vga_mode};
}

void vga_set_screen_mode(enum vga_color mode)
{
	vga_mode = mode;
	for (int y = 0; y < VGA_HEIGHT; y++)
		for (int x = 0; x < VGA_WIDTH; x++)
			VGA_ENTRY(x, y)->mode = vga_mode;
}

void vga_set_mode(enum vga_color mode) { vga_mode = mode; }

void vga_clear(void)
{
	for (int y = 0; y < VGA_HEIGHT; y++)
		for (int x = 0; x < VGA_WIDTH; x++)
			*VGA_ENTRY(x, y) = (vga_entry){.character = 0x00, .mode = vga_mode};
}
