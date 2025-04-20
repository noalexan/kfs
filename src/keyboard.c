#include "keyboard.h"

// typedef enum {
// UNDEFINED = 0,
// KEY_ALPHANUMERIC,
// KEY_CONTROL,
// KEY_NAVIGATION,
// KEY_FUNCTION,
// KEY_SPECIAL
// } KeyCategory;

#define MAX_CLI_LEN 253

static bool caps_lock   = false;
static bool left_shift  = false;
static bool right_shift = false;
static bool left_ctrl   = false;
static bool left_alt    = false;
static bool num_lock    = false;
static bool scroll_lock = false;

scancode_routine_t current_layout[256] = {0};

/////////////////////////////////////////////////////////////////////////////////////////////
// Printable Group

static char keyboard_get_shifted_value(keyboard_key_t key)
{
	bool shift = left_shift || right_shift;
	char ascii = key.value;
	if (key.value >= 'a' && key.value <= 'z') {
		if (shift != caps_lock)
			ascii = key.shifted_value;
	} else if (shift)
		ascii = key.shifted_value;
	return ascii;
}

static void keyboard_printable_handler(keyboard_key_t key)
{
	if (key.undergroup == NUM_PAD && !num_lock && key.shifted_value == 0)
		return;
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
		if (printable_table[i].normal) {
			scancode_routine_t new_entry = {.category = KEY_ALPHANUMERIC,
			                                .handler  = keyboard_printable_handler,
			                                .key      = {.keycode       = i,
			                                             .value         = printable_table[i].normal,
			                                             .shifted_value = printable_table[i].shifted,
			                                             .state_ptr     = NULL,
			                                             .undergroup = printable_table[i].undergroup}};
			current_layout[i]            = new_entry;
		}
	}
}

// Printable Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// Control Group

static bool *keyboard_get_control_global(ControlKeys keyname)
{
	switch (keyname) {
	case LEFT_CTRL:
		return &left_ctrl;
	case LEFT_SHIFT:
		return &left_shift;
	case RIGHT_SHIFT:
		return &right_shift;
	case LEFT_ALT:
		return &left_alt;
	case CAPS_LOCK:
		return &caps_lock;
	case NUM_LOCK:
		return &num_lock;
	case SCROLL_LOCK:
		return &scroll_lock;
	default:
		printk("ControlKeys %d doesn't exist on global control key\n", keyname);
		return NULL;
	}
}
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

key_handler_t keyboard_get_control_handler(uint8_t state)
{
	switch (state) {
	case KEY_PRESS:
		return keyboard_press_handler;
	case KEY_RELEASE:
		return keyboard_release_handler;
	case KEY_LOCK:
		return keyboard_toggle_handler;
	default:
		return NULL;
	}
}

static void keyboard_init_control_group()
{
	for (int i = 0; i < 256; i++) {
		if (control_table[i].keyname) {
			uint8_t            undergroup = control_table[i].undergroup;
			scancode_routine_t new_entry  = {
			     .category = KEY_CONTROL,
			     .handler  = keyboard_get_control_handler(undergroup),
			     .key      = {.undergroup    = undergroup,
			                  .keycode       = i,
			                  .value         = 0,
			                  .shifted_value = 0,
			                  .state_ptr     = keyboard_get_control_global(control_table[i].keyname)}};
			current_layout[i] = new_entry;
		}
	}
}

// Control Group
/////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////
// External API

static group_init_funs_t init_fun[] = {keyboard_init_printable_group, keyboard_init_control_group,
                                       NULL};

// TODO : register dimamically when memory is implemented
void keyboard_bind_key(KeyCategory category, key_handler_t handler, keyboard_key_t key)
{
	if (key.keycode > 256 || !handler)
		kpanic("ERROR: bad parameters to keyboard_register_routine\n");
	current_layout[key.keycode] =
	    (scancode_routine_t){.category = category, .handler = handler, .key = key};
}

// TODO : use free when memory
void keyboard_unbind_key(uint8_t keycode) { current_layout[keycode] = UNDEFINED_KEY; }

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
