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

#define PAGE_SIZE  4096
#define PAGE_SHIFT 12
#define PAGE_MAGIC 0xDEADBEEF

#define PAGE_RESERVED  0b00000001
#define PAGE_BUDDY     0b00000010
#define PAGE_ALLOCATED 0b00000100
#define PAGE_DMA       0b00001000
#define PAGE_LOWMEM    0b00010000
#define PAGE_HIGHMEM   0b00100000
#define PAGE_ZONE_MASK 0b00111000

// Macros

#define PAGE_SET_ZONE(page, zone) ((page)->flags = ((page)->flags & ~PAGE_ZONE_MASK) | (zone))
#define PAGE_IS_DMA(page)         (FLAG_IS_SET((page)->flags, PAGE_DMA))
#define PAGE_IS_LOWMEM(page)      (FLAG_IS_SET((page)->flags, PAGE_LOWMEM))
#define PAGE_IS_HIGHMEM(page)     (FLAG_IS_SET((page)->flags, PAGE_HIGHMEM))
#define PAGE_GET_ZONE(page)       ((page)->flags & PAGE_ZONE_MASK)

#define PAGE_SET_ALLOCATED(page) FLAG_SET((page)->flags, PAGE_ALLOCATED)
#define PAGE_SET_FREE(page)      FLAG_UNSET((page)->flags, PAGE_ALLOCATED)
#define PAGE_BUDDY_CLAIM(page)   FLAG_SET((page)->flags, PAGE_BUDDY)
#define PAGE_BUDDY_UNCLAIM(page) FLAG_UNSET((page)->flags, PAGE_BUDDY)
#define PAGE_IS_UNUSABLE(page)                                                                     \
	(FLAG_IS_SET((page)->flags, PAGE_ALLOCATED) || FLAG_IS_SET((page)->flags, PAGE_RESERVED))
#define PAGE_IS_BUDDY_MANAGED(page) (FLAG_IS_SET((page)->flags, PAGE_BUDDY))
#define PAGE_IS_FREE(page)                                                                         \
	(FLAG_IS_SET((page)->flags, PAGE_BUDDY) && !FLAG_IS_SET((page)->flags, PAGE_ALLOCATED))
#define PAGE_BY_ORDER(order)     (1 << order)
#define PAGE_DATA_IS_MAGIC(page) ((page)->private_data == PAGE_MAGIC)
#define ORDER_TO_BYTES(order)    (PAGE_BY_ORDER(order) * PAGE_SIZE)

// ============================================================================
// STRUCT
// ============================================================================

// Enums

typedef enum { NEXT = 0, PREV } direction;

// Structures

struct page {
	uint32_t  flags;
	uintptr_t private_data;
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
bool      page_addr_is_same_page(uintptr_t addr1, uintptr_t addr2);
uint32_t  page_get_updated_reserved_count(void);
uint32_t  page_get_updated_free_count(void);
uint32_t  page_get_free_count(void);
uint32_t  page_get_reserved_count(void);
