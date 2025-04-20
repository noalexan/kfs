#include "keymap.h"
#include "panic.h"
#include <libft.h>
#include <types.h>

typedef enum {
	UNDEFINED = 0,
	KEY_ALPHANUMERIC,
	KEY_CONTROL,
	KEY_NAVIGATION,
	KEY_FUNCTION,
	KEY_SPECIAL
} KeyCategory;

typedef struct keyboard_key {
	uint8_t  undergroup;
	uint16_t value;
	uint16_t shifted_value;
	uint16_t keycode;
	bool    *state_ptr;
} keyboard_key_t;

typedef struct scancode_routine {
	keyboard_key_t key;
	KeyCategory    category;
	void (*handler)(keyboard_key_t key);
} scancode_routine_t;

typedef void (*group_init_funs_t)(void);
typedef void (*key_handler_t)(keyboard_key_t);

#define UNDEFINED_KEY                                                                              \
	((scancode_routine_t){.key      = {.keycode = 0, .value = 0, .shifted_value = 0},              \
	                      .category = UNDEFINED,                                                   \
	                      .handler  = NULL})

void keyboard_bind_key(KeyCategory category, key_handler_t handler, keyboard_key_t key);
void keyboard_unbind_key(uint8_t keycode);
void keyboard_handle(void);
void keyboard_init_default_layout(void);
// void keyboard_remap_layout(void);
// void keyboard_rebind_as_default(int keycode);

extern scancode_routine_t current_layout[256];

/* -----------------------------------------
 * Printable/Alphanumeric Key group
 */
typedef enum {
	LETTER,
	TOP_KEY,
	SPECIAL,
	SPACE,
	NUM_PAD,
} PrintableKeys;

typedef struct {
	char          normal;
	char          shifted;
	PrintableKeys undergroup;
} printable_entry_t;

static const printable_entry_t printable_table[256] = {
    // letters
    [0x1E] = {'a', 'A', LETTER},
    [0x30] = {'b', 'B', LETTER},
    [0x2E] = {'c', 'C', LETTER},
    [0x20] = {'d', 'D', LETTER},
    [0x12] = {'e', 'E', LETTER},
    [0x21] = {'f', 'F', LETTER},
    [0x22] = {'g', 'G', LETTER},
    [0x23] = {'h', 'H', LETTER},
    [0x17] = {'i', 'I', LETTER},
    [0x24] = {'j', 'J', LETTER},
    [0x25] = {'k', 'K', LETTER},
    [0x26] = {'l', 'L', LETTER},
    [0x32] = {'m', 'M', LETTER},
    [0x31] = {'n', 'N', LETTER},
    [0x18] = {'o', 'O', LETTER},
    [0x19] = {'p', 'P', LETTER},
    [0x10] = {'q', 'Q', LETTER},
    [0x13] = {'r', 'R', LETTER},
    [0x1F] = {'s', 'S', LETTER},
    [0x14] = {'t', 'T', LETTER},
    [0x16] = {'u', 'U', LETTER},
    [0x2F] = {'v', 'V', LETTER},
    [0x11] = {'w', 'W', LETTER},
    [0x2D] = {'x', 'X', LETTER},
    [0x15] = {'y', 'Y', LETTER},
    [0x2C] = {'z', 'Z', LETTER},

    // top keys
    [0x02] = {'1', '!', TOP_KEY},
    [0x03] = {'2', '@', TOP_KEY},
    [0x04] = {'3', '#', TOP_KEY},
    [0x05] = {'4', '$', TOP_KEY},
    [0x06] = {'5', '%', TOP_KEY},
    [0x07] = {'6', '^', TOP_KEY},
    [0x08] = {'7', '&', TOP_KEY},
    [0x09] = {'8', '*', TOP_KEY},
    [0x0A] = {'9', '(', TOP_KEY},
    [0x0B] = {'0', ')', TOP_KEY},

    // specials
    [0x0C] = {'-', '_', SPECIAL},
    [0x0D] = {'=', '+', SPECIAL},
    [0x1A] = {'[', '{', SPECIAL},
    [0x1B] = {']', '}', SPECIAL},
    [0x27] = {';', ':', SPECIAL},
    [0x28] = {'\'', '"', SPECIAL},
    [0x29] = {'`', '~', SPECIAL},
    [0x2B] = {'\\', '|', SPECIAL},
    [0x33] = {',', '<', SPECIAL},
    [0x34] = {'.', '>', SPECIAL},
    [0x35] = {'/', '?', SPECIAL},

    // spaces
    [0x39] = {' ', ' ', SPACE},
    [0x0F] = {'\t', '\t', SPACE},

    // Num pad
    [0x52] = {'0', 0, NUM_PAD},
    [0x4F] = {'1', 0, NUM_PAD},
    [0x50] = {'2', 0, NUM_PAD},
    [0x51] = {'3', 0, NUM_PAD},
    [0x4B] = {'4', 0, NUM_PAD},
    [0x4C] = {'5', 0, NUM_PAD},
    [0x4D] = {'6', 0, NUM_PAD},
    [0x47] = {'7', 0, NUM_PAD},
    [0x48] = {'8', 0, NUM_PAD},
    [0x49] = {'9', 0, NUM_PAD},
    [0x37] = {'*', '*', NUM_PAD},
    [0x4A] = {'-', '-', NUM_PAD},
    [0x4E] = {'+', '+', NUM_PAD},
    [0x53] = {'.', '.', NUM_PAD},
};

/* -----------------------------------------
 * Control Key group
 */
typedef enum {
	KEY_PRESS = 0,
	KEY_REPEAT,
	KEY_RELEASE,
	KEY_LOCK,
} KeyState;

typedef enum {
	LEFT_CTRL,
	LEFT_SHIFT,
	RIGHT_SHIFT,
	LEFT_ALT,
	CAPS_LOCK,
	NUM_LOCK,
	SCROLL_LOCK,
} ControlKeys;

typedef struct {
	ControlKeys keyname;
	KeyState    undergroup;
} control_entry_t;

static const control_entry_t control_table[256] = {
    // Control keys (press)
    [0x1D] = {LEFT_CTRL, KEY_PRESS},
    [0x2A] = {LEFT_SHIFT, KEY_PRESS},
    [0x36] = {RIGHT_SHIFT, KEY_PRESS},
    [0x38] = {LEFT_ALT, KEY_PRESS},
    // Control keys (release)
    [0x9D] = {LEFT_CTRL, KEY_RELEASE},
    [0xAA] = {LEFT_SHIFT, KEY_RELEASE},
    [0xB6] = {RIGHT_SHIFT, KEY_RELEASE},
    [0xB8] = {LEFT_ALT, KEY_RELEASE},
    // Control Toggle Key
    [0xBA] = {CAPS_LOCK, KEY_LOCK},
    [0xC5] = {NUM_LOCK, KEY_LOCK},
    [0xC6] = {SCROLL_LOCK, KEY_LOCK},
    /* TODO :
     * Actually Lock key are toggled on release but need to improve the logic the file doc/idea.md
     * contain a good way to start
     */
};
