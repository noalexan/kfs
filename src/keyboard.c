#include "keyboard.h"
#include "io.h"
#include "layout.h"

TTY               *current_tty;
TTY                ttys[12];
Layout             current_layout_type = QWERTY;
scancode_routine_t current_layout[256] = {0};

/////////////////////////////////////////////////////////////////////////////////////////////
// Printable Group

static char keyboard_get_shifted_value(keyboard_key_t key)
{
	bool shift = left_shift || right_shift;

	if (key.value >= 'a' && key.value <= 'z') {
		return (shift ^ caps_lock) ? key.alt_value : key.value;
	} else if (key.category == KEY_ALPHANUMERIC) {
		return shift ? key.alt_value : key.value;
	} else if (key.category == KEY_NAVIGATION && key.undergroup == NUM_PAD && num_lock) {
		return key.alt_value;
	}
	return key.value;
}

static void keyboard_printable_handler(keyboard_key_t key)
{
	char   ascii   = keyboard_get_shifted_value(key);
	size_t cmd_len = ft_strlen(current_tty->cli);

	if (cmd_len < MAX_CMD_LEN) {
		current_tty->cli[cmd_len]     = ascii;
		current_tty->cli[cmd_len + 1] = 0;
	}

	printk("%c", ascii);
}

// Printable Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Control Group

static void keyboard_toggle_handler(keyboard_key_t key)
{
	if (key.state_ptr)
		*(key.state_ptr) = !*(key.state_ptr);
}

static void keyboard_press_handler(keyboard_key_t key)
{
	if (key.state_ptr && *(key.state_ptr) == false)
		*(key.state_ptr) = true;
}

static void keyboard_release_handler(keyboard_key_t key)
{
	if (key.state_ptr && *(key.state_ptr) == true)
		*(key.state_ptr) = false;
}

static key_handler_t keyboard_get_control_handler(uint8_t state)
{
	switch (state) {
	case PRESS:
		return keyboard_press_handler;
	case RELEASE:
		return keyboard_release_handler;
	case TOGGLE:
		return keyboard_toggle_handler;
	default:
		return NULL;
	}
}

// Control Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Navigation Group

static void keyboard_navigation_handler(keyboard_key_t key)
{
	if (!*(key.state_ptr))
		tty_switch_color(key.value);
	else
		keyboard_printable_handler(key);
}

// Navigation Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Function Group

static void keyboard_function_handler(keyboard_key_t key)
{
	tty_save(current_tty);
	current_tty = ttys + key.value;
	tty_load(current_tty);
}

// Function Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Special Group

static void keyboard_enter_handler(keyboard_key_t key)
{
	(void)key;
	tty_cli_handle_nl();
}

static void keyboard_escape_handler(keyboard_key_t key)
{
	(void)key;
	shutdown();
}

static void keyboard_backspace_handler(keyboard_key_t key)
{
	int len = ft_strlen(current_tty->cli);

	(void)key;

	if (len) {
		current_tty->cli[len - 1] = 0;

		if (g_cursor.x == 0) {
			if (g_cursor.y > 0) {
				g_cursor.x = VGA_WIDTH - 1;
				g_cursor.y--;
			}
		}

		else {
			g_cursor.x--;
		}

		vga_set_cursor_position(g_cursor.x, g_cursor.y);
		VGA_ENTRY(g_cursor.x, g_cursor.y)->character = 0;
	}
}

static key_handler_t keyboard_get_special_handler(uint8_t undergroup)
{
	switch (undergroup) {
	case ENTER:
		return keyboard_enter_handler;
	case ESCAPE:
		return keyboard_escape_handler;
	case BACKSPACE:
		return keyboard_backspace_handler;
	default:
		return NULL;
	}
}

// Special Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Internal API

static key_handler_t keyboard_get_default_key_handler(keyboard_key_t key)
{
	switch (key.category) {
	case KEY_ALPHANUMERIC:
		return keyboard_printable_handler;
	case KEY_NAVIGATION:
		return keyboard_navigation_handler;
	case KEY_FUNCTION:
		return keyboard_function_handler;
	case KEY_CONTROL:
		return keyboard_get_control_handler(key.undergroup);
	case KEY_SPECIAL:
		return keyboard_get_special_handler(key.undergroup);
	default:
		kpanic("Error: key Invalid when called keyboard_get_key_handler fun\n");
		break;
	}
	return NULL;
}

static void keyboard_init_default_table(void)
{
	keyboard_key_t *groups[] = {printable_keys, control_keys, navigation_keys,
	                            function_keys,  special_keys, NULL};

	for (uint32_t g = 0; groups[g] != NULL; g++) {
		for (uint32_t i = 0; groups[g][i].keycode != UNDEFINED; i++) {
			uint8_t kc            = groups[g][i].keycode;
			default_key_table[kc] = groups[g][i];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// External API

void keyboard_switch_layout(Layout new_layout)
{
	if (current_layout_type == new_layout) {
		printk("\nkeyboard: Layout already set");
		return;
	} else if (new_layout == QWERTY) {
		printk("\nkeyboard: Switching layout to QWERTY");
		keyboard_remap_layout(default_key_table, 256);
	} else if (new_layout == AZERTY) {
		printk("\nkeyboard: Switching layout to AZERTY");
		keyboard_remap_layout(azerty_layout, STOP_WHEN_UNDEFINED);
	}
	current_layout_type = new_layout;
}

void keyboard_remap_layout(keyboard_key_t *table, uint32_t size)
{
	if (size == STOP_WHEN_UNDEFINED) {
		uint32_t i = 0;
		while (table[i].keycode != UNDEFINED) {
			key_handler_t handler = keyboard_get_default_key_handler(table[i]);
			keyboard_bind_key(handler, table[i]);
			i++;
		}
	} else {
		for (uint32_t i = 0; i <= size; i++) {
			if (table[i].keycode != UNDEFINED) {
				key_handler_t handler = keyboard_get_default_key_handler(table[i]);
				keyboard_bind_key(handler, table[i]);
			}
		}
	}
}

// TODO : register dimamically when memory is implemented
void keyboard_bind_key(key_handler_t handler, keyboard_key_t key)
{
	if (key.keycode > 256 || !handler)
		kpanic("ERROR: bad parameters to keyboard_register_routine\n");
	current_layout[key.keycode] = (scancode_routine_t){.handler = handler, .key = key};
}

// TODO : use free when memory
void keyboard_unbind_key(uint8_t keycode) { current_layout[keycode] = UNDEFINED_ROUTINE; }

void keyboard_init(void)
{
	for (int i = 0; i < 256; ++i)
		current_layout[i] = UNDEFINED_ROUTINE;
	keyboard_init_default_table();
	keyboard_remap_layout(default_key_table, KEY_MAX);
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
