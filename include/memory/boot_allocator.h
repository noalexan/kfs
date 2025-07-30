#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include <kernel/mb2_info.h>
#include <types.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

enum allocator_state { ACTIVE = 0, FROZEN };

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY, HOLES_MEMORY };

// Structures

struct region_s {
	uintptr_t start;
	uintptr_t end;
};

struct boot_allocator;

// Typedefs

typedef struct region_s       region_t;
typedef struct boot_allocator boot_allocator_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// EXTERNAL APIs
// ============================================================================

bool      boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type);
void      boot_allocator_freeze(void);
void      boot_allocator_printer(void);
void      boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);
uint32_t  boot_allocator_get_region_count(enum mem_type type);
region_t *boot_allocator_get_region(enum mem_type type);
