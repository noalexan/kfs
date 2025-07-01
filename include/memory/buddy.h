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

#define FREE      0
#define ALLOCATED 1

#define KiB_SIZE (1UL << 10)
#define MiB_SIZE (1UL << 20)
#define GiB_SIZE (1UL << 30)

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

typedef enum {
	LOWMEM_ZONE = 0,
	DMA_ZONE,
	HIGHMEM_ZONE,
	INVALID_ZONE,
} zone_type;

typedef enum {
	MIGRATE_MOVABLE = 0,
	// Not Implemented
	MIGRATE_UNMOVABLE,
	MIGRATE_RECLAIMABLE,
} migration_type;

// Structures

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// (aucune variable globale à exposer ici)

// ============================================================================
// EXTERNAL APIs
// ============================================================================

uintptr_t *buddy_alloc_pages(size_t size, zone_type zone);
void       buddy_print(zone_type zone);
void       buddy_free_block(void *ptr);
void       buddy_init(void);
void       debug_buddy(void);
