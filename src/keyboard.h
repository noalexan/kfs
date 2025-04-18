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

void keyboard_register_routine(KeyCategory    category, void (*handler)(keyboard_key_t *value),
                               keyboard_key_t key);
void keyboard_unregister_routine(uint8_t keycode);
void keyboard_handle(void);
void keyboard_init_default_layout(void);

// void keyboard_remap_layout(void);