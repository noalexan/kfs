#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

#include <types.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define MAX_REGIONS 256
#define END         0x00000000

// Macros

// Only in if statement
#define BOOT_ALLOCATOR_IS_REGION_UNMAPPED(start, end)                                              \
	(!(boot_allocator_overlaps((start), (end), RESERVED_MEMORY) ||                                 \
	   boot_allocator_overlaps((start), (end), FREE_MEMORY)))

// ============================================================================
// STRUCT
// ============================================================================

// Enums

enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY };
enum allocator_state { ACTIVE = 0, FROZEN };

// Structures

typedef struct region_s {
	uintptr_t start;
	uintptr_t end;
} region_t;

typedef struct boot_allocator {
	bool     state;
	uint32_t free_count;
	region_t free_regions[MAX_REGIONS];
	uint32_t reserved_count;
	region_t reserved_regions[MAX_REGIONS];
} boot_allocator_t;

// Typedefs

typedef void (*regions_foreach_fn)(region_t *regions);

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern boot_allocator_t bootmem;

// ============================================================================
// EXTERNAL APIs
// ============================================================================
