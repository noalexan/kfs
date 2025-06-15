#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

#include <acpi.h>
#include <memory/boot_allocator.h>
#include <types.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define END               0x00000000
#define REGION_TYPE_COUNT 3
#define MAX_REGIONS       128

// Macros

#define BOOT_ALLOCATOR_SORT_AND_MERGE(regions, count)                                              \
	do {                                                                                           \
		boot_allocator_sort_regions((regions), (count));                                           \
		(count) = boot_allocator_merge_contiguous_regions((regions), (count));                     \
	} while (0)

// Only in if statement
#define BOOT_ALLOCATOR_IS_REGION_UNMAPPED(start, end)                                              \
	(!(boot_allocator_overlaps((start), (end), RESERVED_MEMORY) ||                                 \
	   boot_allocator_overlaps((start), (end), FREE_MEMORY)))

#define BOOT_ALLOC_FREE_COUNT(alloc)     ((alloc)->count[FREE_MEMORY])
#define BOOT_ALLOC_RESERVED_COUNT(alloc) ((alloc)->count[RESERVED_MEMORY])
#define BOOT_ALLOC_HOLE_COUNT(alloc)     ((alloc)->count[HOLES_MEMORY])

#define BOOT_ALLOC_FREE_REGIONS(alloc)     ((alloc)->regions[FREE_MEMORY])
#define BOOT_ALLOC_RESERVED_REGIONS(alloc) ((alloc)->regions[RESERVED_MEMORY])
#define BOOT_ALLOC_HOLE_REGIONS(alloc)     ((alloc)->regions[HOLES_MEMORY])

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

typedef struct boot_allocator {
	bool     state;
	uint32_t count[REGION_TYPE_COUNT];
	region_t regions[REGION_TYPE_COUNT][MAX_REGIONS];
} boot_allocator_t;

// Typedefs

typedef void (*regions_foreach_fn)(region_t *regions);

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern boot_allocator_t bootmem;
static region_t         all_reg_g[MAX_REGIONS * REGION_TYPE_COUNT];

// ============================================================================
// INTERNAL APIs
// ============================================================================
