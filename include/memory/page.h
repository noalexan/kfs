#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include <types.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define MAX_PAGES  (1UL << 20)
#define PAGE_MAGIC 0xDEADBEEF

#define PAGE_RESERVED  0b00000001
#define PAGE_BUDDY     0b00000010
#define PAGE_ALLOCATED 0b00000100
#define PAGE_DMA       0b00001000
#define PAGE_LOWMEM    0b00010000
#define PAGE_HIGHMEM   0b00100000
#define PAGE_ZONE_MASK 0b00111000

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

typedef enum { NEXT = 0, PREV } direction;

// Structures

struct page {
	struct list_head node;
	uint32_t         flags;
	uintptr_t        private_data;
};

// Typedefs

typedef struct page page_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern uint32_t total_pages;
extern uint32_t total_RAM;
extern page_t  *page_descriptors;

// ============================================================================
// EXTERNAL APIs
// ============================================================================

void      page_print_info(page_t *page);
void      page_descriptor_init(void);
uint32_t  page_to_index(page_t *page);
uintptr_t page_to_phys(page_t *page);
page_t   *page_index_to_page(uint32_t idx);
page_t   *page_addr_to_page(uintptr_t addr);
page_t   *page_addr_to_usable(uintptr_t addr, bool direction);
uint32_t  page_get_updated_reserved_count(void);
uint32_t  page_get_updated_free_count(void);
uint32_t  page_get_free_count(void);
uint32_t  page_get_reserved_count(void);
bool      page_addr_is_same_page(uintptr_t addr1, uintptr_t addr2);
