#include <io.h>
#include <libft.h>
#include <vga.h>

vga_entry *const vga_buffer = (vga_entry *)0xb8000;
u8 vga_screen_mode = 0x00;
struct s_cursor cursor = {0, 0};

void vga_update_cursor(int x, int y)
{
	u16 pos = y * VGA_SCREEN_WIDTH + x;
	cursor = (struct s_cursor){x, y};

	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8)(pos & 0xFF));

	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

void vga_put_char(int x, int y, char c)
{
	VGA_ENTRY(x % VGA_SCREEN_WIDTH, y % VGA_SCREEN_HEIGTH)->character = c;
}

void scroll_down(void)
{
	for (int y = 0; y < (VGA_SCREEN_HEIGTH - 1); y++)
		ft_memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_SCREEN_WIDTH * 2);
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
