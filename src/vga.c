#include <io.h>
#include <vga.h>

vga_entry *const vga_buffer = (vga_entry *)0xb8000;
u8 vga_screen_mode = 0x00;

void vga_update_cursor(int x, int y)
{
	u16 pos = y * VGA_SCREEN_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8)(pos & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

void vga_enable_cursor(u8 cursor_start, u8 cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

static void *memcpy(void *dest, void *src, int length)
{
	for (int i = 0; i < length; i++)
		((u8 *)dest)[i] = ((u8 *)src)[i];
	return dest;
}

void vga_put_char(int x, int y, char c)
{
	VGA_ENTRY(x % VGA_SCREEN_WIDTH, y % VGA_SCREEN_HEIGTH)->character = c;
}

void scroll_down(void)
{
	for (int y = 0; y < (VGA_SCREEN_HEIGTH - 1); y++)
		memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_SCREEN_WIDTH * 2);
	for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
		*VGA_ENTRY(x, VGA_SCREEN_HEIGTH - 1) = (vga_entry){0, vga_screen_mode};
}

void set_screen_mode(u8 mode)
{
	vga_screen_mode = mode;
	for (int y = 0; y < VGA_SCREEN_HEIGTH; y++)
		for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
			VGA_ENTRY(x, y)->mode = vga_screen_mode;
}

void clear_screen(void)
{
	for (int y = 0; y < VGA_SCREEN_HEIGTH; y++)
		for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
			VGA_ENTRY(x, y)->character = 0x00;
}
