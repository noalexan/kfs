#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include "page.h"
#include <types.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define MAX_ORDER     10
#define MAX_MIGRATION 1

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

typedef enum {
	ORDER_4KIB = 0,
	ORDER_8KIB,
	ORDER_16KIB,
	ORDER_32KIB,
	ORDER_64KIB,
	ORDER_128KIB,
	ORDER_256KIB,
	ORDER_512KIB,
	ORDER_1MIB,
	ORDER_2MIB,
	ORDER_4MIB,
	BAD_ORDER,
} order_size;

// Structures

struct buddy_free_area {
	struct list_head free_list[MAX_MIGRATION];
	uint32_t         nr_free;
};

struct buddy_allocator {
	struct buddy_free_area areas[MAX_ORDER + 1];
};

// Typedefs

typedef struct buddy_allocator buddy_allocator_t;

// Structures

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// (aucune variable globale à exposer ici)

// ============================================================================
// EXTERNAL APIs
// ============================================================================
