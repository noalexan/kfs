#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include <register.h>
#include <types.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define PAGE_SIZE 4096

#define TO_FREE false
#define TO_KEEP true

#define FREE      0
#define ALLOCATED 1

#define KiB_SIZE (1UL << 10)
#define MiB_SIZE (1UL << 20)
#define GiB_SIZE (1UL << 30)

#define KERNEL_VADDR_BASE 0xC0000000

/* Zones and migrations types */

#define DMA_START 0x00000000
#define DMA_END   0x00FFFFFF // 16 Mo - 1 (0x01000000 - 1)
// Zone NORMAL (lowmem) : 16 Mo -> 896 Mo
#define LOWMEM_START 0x01000000 // 16 Mo
#define LOWMEM_END   0x37FFFFFF // 896 Mo - 1 (0x38000000 - 1)
// Zone HIGHMEM : 896 Mo -> max addr
#define HIGHMEM_START 0x38000000 // 896 Mo
#define HIGHMEM_END   0xFFFFFFFF // max addr

#define MAX_MIGRATION 1
#define MAX_ZONE      3

#define VMALLOC_START 0xF8000000
#define VMALLOC_END   0xFFFFFFFF

// Macros
#define PHYS_TO_VIRT_LINEAR(p_addr) ((void *)((uintptr_t)(p_addr) + KERNEL_VADDR_BASE))
#define VIRT_TO_PHYS_LINEAR(v_addr) ((uintptr_t)((v_addr) - KERNEL_VADDR_BASE))

// ============================================================================
// STRUCT
// ============================================================================

// Enums

enum allocator_state { ACTIVE = 0, FROZEN };

typedef enum { LOWMEM_ZONE = 0, DMA_ZONE, HIGHMEM_ZONE, INVALID_ZONE } zone_type;

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

// ============================================================================
// EXTERNAL APIs
// ============================================================================

uintptr_t *buddy_alloc_pages(size_t size, zone_type zone);
void       buddy_free_block(void *ptr);
void       buddy_init(void);
void       vmm_finalize(void);
