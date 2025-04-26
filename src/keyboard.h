#pragma once

#include "panic.h"
#include "tty.h"
#include <libft.h>
#include <types.h>

#define UNDEFINED           0
#define STOP_WHEN_UNDEFINED 0
#define KEY_MAX             0xff

#define UNDEFINED_KEY                                                                              \
	((keyboard_key_t){.value      = UNDEFINED,                                                     \
	                  .alt_value  = UNDEFINED,                                                     \
	                  .keycode    = UNDEFINED,                                                     \
	                  .category   = UNDEFINED,                                                     \
	                  .undergroup = UNDEFINED,                                                     \
	                  .state_ptr  = NULL})

#define UNDEFINED_ROUTINE ((scancode_routine_t){.key = UNDEFINED_KEY, .handler = NULL})

typedef enum {
	KEY_ALPHANUMERIC = 0,
	KEY_CONTROL,
	KEY_NAVIGATION,
	KEY_FUNCTION,
	KEY_SPECIAL
} KeyCategory;

typedef enum {
	NONE = 0,
	LETTER,
	TOP_KEY,
	PUNCTUATION,
	SPACE,
	NUM_PAD,
	PRESS,
	RELEASE,
	TOGGLE,
	BACKSPACE,
	ENTER,
	ESCAPE
} KeyUndergroup;

typedef enum { QWERTY = 0, AZERTY } Layout;

typedef struct keyboard_key {
	uint16_t      value;
	uint16_t      alt_value;
	uint16_t      keycode;
	KeyCategory   category;
	KeyUndergroup undergroup;
	bool         *state_ptr;
} keyboard_key_t;

typedef void (*group_init_funs_t)(void);
typedef void (*key_handler_t)(keyboard_key_t);

typedef struct scancode_routine {
	keyboard_key_t key;
	key_handler_t  handler;
} scancode_routine_t;

extern scancode_routine_t current_layout[256];

void keyboard_bind_key(key_handler_t handler, keyboard_key_t key);
void keyboard_unbind_key(uint8_t keycode);
void keyboard_handle(void);
void keyboard_init(void);
void keyboard_remap_layout(keyboard_key_t *table, uint32_t size);
void keyboard_switch_layout(Layout new_layout);
// void keyboard_rebind_as_default(int keycode);
