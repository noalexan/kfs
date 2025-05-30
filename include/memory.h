#pragma once
// ============================================================================
// IMCLUDES
// ============================================================================

#include "mb2_info.h"
#include <types.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Define

#define PAGE_SIZE  4096
#define PAGE_SHIFT 12
#define MAX_ORDER  11

#define KiB_SIZE (1UL << 10)
#define MiB_SIZE (1UL << 20)
#define GiB_SIZE (1UL << 30)

// Flags for page
#define PAGE_RESERVED  0b00000001
#define PAGE_BUDDY     0b00000010
#define PAGE_ALLOCATED 0b00000100

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// ENUM

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
	ORDER_UNSET,
} order_type;

enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY, HOLES_MEMORY };
enum allocator_state { ACTIVE = 0, FROZEN };

// STRUCT

// Forward declarations for internals types

struct boot_allocator;
typedef struct boot_allocator boot_allocator_t;

struct page;
typedef struct page page_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern boot_allocator_t bootmem;
extern uint32_t         total_pages;
extern page_t          *page_descriptors;
extern uint32_t         total_RAM;

// ============================================================================
// EXTERNAL APIs
// ============================================================================

// boot_allocator.c

void *boot_alloc(uint32_t size);
void  boot_allocator_printer(void);
void  boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);
bool  boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type);

// page.c

void page_descriptor_init(void);

// buddy.c
