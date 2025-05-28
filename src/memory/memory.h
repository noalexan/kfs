#pragma once
// ============================================================================
// IMCLUDES
// ============================================================================

#include "boot_allocator.h"
#include "buddy.h"

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Define

#define PAGE_SIZE  4096
#define PAGE_SHIFT 12
#define MAX_ORDER  11

#define PAGE_FREE      0x01
#define PAGE_BUDDY     0x02
#define PAGE_ALLOCATED 0x04

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
	ORDER_4MIB
} OrderType;

// STRUCT

struct page {
	uint32_t     flags;
	uint32_t     order;
	struct page *next;
	uint32_t     reserved;
};

typedef struct page page_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern boot_allocator_t  bootmem;
extern buddy_allocator_t buddy[MAX_ORDER];
extern page_t           *page_descriptors;
