#include "keymap.h"
#include "panic.h"
#include <types.h>

typedef enum {
	UNDEFINED = 0,
	KEY_ALPHANUMERIC,
	KEY_NAVIGATION,
	KEY_CONTROL,
	KEY_FUNCTION,
	KEY_SPECIAL
} KeyCategory;

typedef enum {
	KEY_PRESS,
	KEY_REPEAT,
	KEY_REALESE,
} KeyState;

typedef struct keyboard_key {
	uint16_t value;
	uint16_t shifted_value;
	uint16_t keycode;
} keyboard_key_t;

typedef struct scancode_routine {

	keyboard_key_t key;
	KeyCategory    category;
	void (*handler)(keyboard_key_t key);
	// KeyState    state;
} scancode_routine_t;

typedef void (*group_init_funs_t)(void);

#define UNDEFINED_KEY                                                                              \
	((scancode_routine_t){.key      = {.keycode = 0, .value = 0, .shifted_value = 0},              \
	                      .category = UNDEFINED,                                                   \
	                      .handler  = NULL})

extern scancode_routine_t current_layout[256];

void keyboard_register_routine(KeyCategory    category, void (*handler)(keyboard_key_t value),
                               keyboard_key_t key);
void keyboard_unregister_routine(uint8_t keycode);
void keyboard_handle(void);
void keyboard_init_default_layout(void);

// void keyboard_remap_layout(void);

typedef struct {
	char normal;
	char shifted;
} printable_entry_t;

static const printable_entry_t printable_table[256] = {
    // letters
    [0x1E] = {'a', 'A'},
    [0x30] = {'b', 'B'},
    [0x2E] = {'c', 'C'},
    [0x20] = {'d', 'D'},
    [0x12] = {'e', 'E'},
    [0x21] = {'f', 'F'},
    [0x22] = {'g', 'G'},
    [0x23] = {'h', 'H'},
    [0x17] = {'i', 'I'},
    [0x24] = {'j', 'J'},
    [0x25] = {'k', 'K'},
    [0x26] = {'l', 'L'},
    [0x32] = {'m', 'M'},
    [0x31] = {'n', 'N'},
    [0x18] = {'o', 'O'},
    [0x19] = {'p', 'P'},
    [0x10] = {'q', 'Q'},
    [0x13] = {'r', 'R'},
    [0x1F] = {'s', 'S'},
    [0x14] = {'t', 'T'},
    [0x16] = {'u', 'U'},
    [0x2F] = {'v', 'V'},
    [0x11] = {'w', 'W'},
    [0x2D] = {'x', 'X'},
    [0x15] = {'y', 'Y'},
    [0x2C] = {'z', 'Z'},
    // top keys
    [0x02] = {'1', '!'},
    [0x03] = {'2', '@'},
    [0x04] = {'3', '#'},
    [0x05] = {'4', '$'},
    [0x06] = {'5', '%'},
    [0x07] = {'6', '^'},
    [0x08] = {'7', '&'},
    [0x09] = {'8', '*'},
    [0x0A] = {'9', '('},
    [0x0B] = {'0', ')'},
    // specials
    [0x0C] = {'-', '_'},
    [0x0D] = {'=', '+'},
    [0x1A] = {'[', '{'},
    [0x1B] = {']', '}'},
    [0x27] = {';', ':'},
    [0x28] = {'\'', '"'},
    [0x29] = {'`', '~'},
    [0x2B] = {'\\', '|'},
    [0x33] = {',', '<'},
    [0x34] = {'.', '>'},
    [0x35] = {'/', '?'},
    // spaces
    [0x39] = {' ', ' '},
    [0x0F] = {'\t', '\t'},
    // Num pad
    [0x52] = {'0', '0'},
    [0x4F] = {'1', '1'},
    [0x50] = {'2', '2'},
    [0x51] = {'3', '3'},
    [0x4B] = {'4', '4'},
    [0x4C] = {'5', '5'},
    [0x4D] = {'6', '6'},
    [0x47] = {'7', '7'},
    [0x48] = {'8', '8'},
    [0x49] = {'9', '9'},
    [0x37] = {'*', '*'},
    [0x4A] = {'-', '-'},
    [0x4E] = {'+', '+'},
    [0x53] = {'.', '.'},
};
