#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

#include "io.h"
#include <drivers/tty.h>
#include <drivers/vga.h>
#include <libft.h>
#include <stdarg.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define KERN_EMERG   "0"
#define KERN_ALERT   "1"
#define KERN_CRIT    "2"
#define KERN_ERR     "3"
#define KERN_WARNING "4"
#define KERN_NOTICE  "5"
#define KERN_INFO    "6"
#define KERN_DEBUG   "7"
#define KERN_DEFAULT ""
#define KERN_CONT    "c"

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// EXTERNAL APIs
// ============================================================================

void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void vga_set_char(int x, int y, char c);
void vga_scroll_down(void);
