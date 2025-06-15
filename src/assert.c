#include <vga.h>

void __assert_fail(const char *expr, const char *file, size_t line)
{
	enum vga_color mode = vga_get_mode();
	vga_set_mode(VGA_COLOR(VGA_COLOR_BLACK, VGA_COLOR_RED));
	vga_printf("Assertion failed:%s:%u: %s", file, line, expr);
	vga_set_mode(mode);
	vga_printf("\n");
}
