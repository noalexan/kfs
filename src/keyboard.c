#include "keyboard.h"

// typedef enum {
// UNDEFINED = 0,
// KEY_ALPHANUMERIC,
// KEY_NAVIGATION,
// KEY_CONTROL,
// KEY_FUNCTION,
// KEY_SPECIAL
// } KeyCategory;

static bool caps_lock   = false;
static bool left_shift  = false;
static bool right_shift = false;

#define MAX_CLI_LEN 253

scancode_routine_t current_layout[256] = {0};

/////////////////////////////////////////////////////////////////////////////////////////////
// Printable Group

static char keyboard_get_shifted_value(keyboard_key_t key)
{
	bool shift = left_shift || right_shift;
	char ascii = key.value;
	if (key.value >= 'a' && key.value <= 'z') {
		if (shift ^ caps_lock)
			ascii = key.shifted_value;
	} else if (shift)
		ascii = key.shifted_value;
	return ascii;
}

static void keyboard_printable_handler(keyboard_key_t key)
{
	char     ascii   = keyboard_get_shifted_value(key);
	char    *cmd     = &current_tty->cli[3];
	uint32_t cmd_len = ft_strlen(cmd);
	if (cmd_len < MAX_CLI_LEN)
		cmd[cmd_len] = ascii;
	printk("%c", ascii);
}

static void keyboard_init_printable_group()
{
	for (int i = 0; i < 256; i++) {
		if (printable_table[i]) {
			scancode_routine_t new_entry = {
			    .category = KEY_ALPHANUMERIC,
			    .handler  = keyboard_printable_handler,
			    .key      = {
			             .keycode       = i,
			             .value         = printable_table[i],
			             .shifted_value = i < 64 ? printable_shifted_table[i] : 0,
                }};
			current_layout[i] = new_entry;
		}
	}
}

// Printable Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// External API

static group_init_funs_t init_fun[] = {keyboard_init_printable_group, NULL};

// TODO : register dimamically when memory is implemented
void keyboard_register_routine(KeyCategory    category, void (*handler)(keyboard_key_t *value),
                               keyboard_key_t key)
{
	if (key.keycode > 256 || !handler)
		kpanic("ERROR: bad parameters to keyboard_register_routine\n");
	current_layout[key.keycode] =
	    (scancode_routine_t){.category = category, .handler = handler, .key = key};
}

// TODO : use free when memory
void keyboard_unregister_routine(uint8_t keycode) { current_layout[keycode] = UNDEFINED_KEY; }

void keyboard_init_default_layout(void)
{
	for (int i = 0; i < 256; ++i)
		current_layout[i] = UNDEFINED_KEY;
	for (int i = 0; init_fun[i]; i++) {
		init_fun[i]();
	}
}
// TODO: improve to add statement handling + init dynamically when memory is implemented
void keyboard_handle(void)
{
	if (inb(0x64) & 0x01) {          // read status
		uint8_t keycode = inb(0x60); // read data
		if (current_layout[keycode].handler) {
			keyboard_key_t key = current_layout[keycode].key;
			current_layout[keycode].handler(key);
		}
	}
}
