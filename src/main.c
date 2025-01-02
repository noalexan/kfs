#define VGA_SCREEN_WIDTH 80
#define VGA_SCREEN_HEIGTH 25

#define VGA_ENTRY(x, y) (vga_buffer + ((y) * VGA_SCREEN_WIDTH + x))

typedef unsigned char u8;

u8 vga_screen_mode;

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

struct s_cursor {
	u8 x;
	u8 y;
};

typedef struct __attribute__((packed)) s_vga_entry {
	u8 character;
	u8 mode;
} vga_entry;

static vga_entry *const vga_buffer = (void *)0xb8000;

static void *memcpy(void *dest, void *src, int length)
{
	for (int i = 0; i < length; i++)
		((u8 *)dest)[i] = ((u8 *)src)[i];
	return dest;
}

// static void *memset(void *buffer, u8 byte, int length)
// {
// 	for (int i = 0; i < length; i++)
// 		*((u8 *)buffer) = byte;
// 	return buffer;
// }

static void scroll_down(void)
{
	for (int y = 0; y < (VGA_SCREEN_HEIGTH - 1); y++)
		memcpy(VGA_ENTRY(0, y), VGA_ENTRY(0, y + 1), VGA_SCREEN_WIDTH * 2);
	for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
		*VGA_ENTRY(x, VGA_SCREEN_HEIGTH - 1) = (vga_entry){0, vga_screen_mode};
}

static void vga_put_char(int x, int y, char c)
{
	VGA_ENTRY(x % VGA_SCREEN_WIDTH, y % VGA_SCREEN_HEIGTH)->character = c;
}

static void put_char(char c)
{
	static struct s_cursor cursor = {0, 0};

	if (cursor.x >= VGA_SCREEN_WIDTH) {
		cursor.y += cursor.x / VGA_SCREEN_WIDTH;
		cursor.x %= VGA_SCREEN_WIDTH;
	}

	while (cursor.y >= VGA_SCREEN_HEIGTH) {
		scroll_down();
		cursor.y--;
	}

	vga_put_char(cursor.x++, cursor.y, c);
}

static void print(const char *str)
{
	for (int i = 0; str[i]; i++)
		put_char(str[i]);
}

static void set_screen_mode(u8 mode)
{
	vga_screen_mode = mode;
	for (int y = 0; y < VGA_SCREEN_HEIGTH; y++)
		for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
			VGA_ENTRY(x, y)->mode = vga_screen_mode;
}

static void clear_screen(void)
{
	for (int y = 0; y < VGA_SCREEN_HEIGTH; y++)
		for (int x = 0; x < VGA_SCREEN_WIDTH; x++)
			VGA_ENTRY(x, y)->character = 0x00;
}

void kernel_main(void)
{
	char str[] = "0aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

	clear_screen();
	set_screen_mode(FOREGROUND_WHITE | BACKGROUND_BLUE);

	for (int i = 0; i < 30; i++) {
		print(str);
		str[0]++;
	}

	print("Hello, World!");

	while (1);
}
