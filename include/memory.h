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

#define MAX_REGIONS 128

#define PAGE_SIZE  4096
#define PAGE_SHIFT 12
#define MAX_ORDER  10

#define KiB_SIZE (1UL << 10)
#define MiB_SIZE (1UL << 20)
#define GiB_SIZE (1UL << 30)

// Flags for page
#define PAGE_RESERVED  0b00000001
#define PAGE_BUDDY     0b00000010
#define PAGE_ALLOCATED 0b00000100

// Currently is useless but here for the scalability
#define ZONE_TYPES  1
#define NORMAL_ZONE 0
// Not Implemented
#define DMA_ZONE   1
#define DMA32_ZONE 2

#define MIGRATION_TYPES 1
#define MIGRATE_MOVABLE 0
// Not Implemented
#define MIGRATE_UNMOVABLE   1
#define MIGRATE_RECLAIMABLE 2

// End of useless things

// Macros

#define PAGE_SET_ALLOCATED(page) FLAG_SET((page)->flags, PAGE_ALLOCATED)
#define PAGE_SET_FREE(page)      FLAG_UNSET((page)->flags, PAGE_ALLOCATED)
#define PAGE_BUDDY_CLAIM(page)   FLAG_SET((page)->flags, PAGE_BUDDY)
#define PAGE_BUDDY_UNCLAIM(page) FLAG_UNSET((page)->flags, PAGE_BUDDY)
#define PAGE_IS_UNUSABLE(page)                                                                     \
	(FLAG_IS_SET((page)->flags, PAGE_ALLOCATED) || FLAG_IS_SET((page)->flags, PAGE_RESERVED))
#define PAGE_IS_BUDDY_MANAGED(page) (FLAG_IS_SET((page)->flags, PAGE_BUDDY))
#define PAGE_IS_FREE(page)                                                                         \
	(FLAG_IS_SET((page)->flags, PAGE_BUDDY) && !FLAG_IS_SET((page)->flags, PAGE_ALLOCATED))

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
	BAD_ORDER,
} order_size;

enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY, HOLES_MEMORY };
enum allocator_state { ACTIVE = 0, FROZEN };

// STRUCT

struct region_s {
	uintptr_t start;
	uintptr_t end;
};

struct list_head {
	struct list_head *next, *prev;
};

struct buddy_free_area {
	struct list_head free_list[MIGRATION_TYPES];
	uint32_t         nr_free;
};

struct buddy_allocator {
	struct buddy_free_area areas[MAX_ORDER + 1];
};

struct page {
	uint32_t  flags;
	uintptr_t private_data;
};

// TYPEDEF

typedef struct region_s        region_t;
typedef struct buddy_allocator buddy_allocator_t;
typedef struct page            page_t;
typedef void (*pages_foreach_fn)(page_t *page, void *data);

// Forward declarations for internals types

// boot_allocator.c

struct boot_allocator;
typedef struct boot_allocator boot_allocator_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern uint32_t          total_pages;
extern uint32_t          total_RAM;
extern page_t           *page_descriptors;
extern buddy_allocator_t buddy[ZONE_TYPES];

// ============================================================================
// EXTERNAL APIs
// ============================================================================

// boot_allocator.c

void    *boot_alloc(uint32_t size);
void     boot_allocator_printer(void);
void     boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);
uint32_t boot_allocator_get_all_free(uintptr_t *dst, uint32_t size);
bool     boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type);
// Getter
region_t *boot_allocator_get_region(enum mem_type type);
uint32_t  boot_allocator_get_region_count(enum mem_type type);
// Setter
void boot_allocator_freeze(void);

// page.c

uint32_t  page_to_index(page_t *page);
uintptr_t page_to_phys(page_t *page);
page_t   *page_index_to_page(uint32_t idx);
page_t   *page_addr_to_page(uintptr_t addr);
bool      page_addr_is_same_page(uintptr_t addr1, uintptr_t addr2);
void      page_print_info(page_t *page);
void      page_descriptor_init(void);

uint32_t page_get_updated_reserved_count(void);
uint32_t page_get_updated_free_count(void);
void     page_descriptor_foreach(pages_foreach_fn handler, void *data);
uint32_t page_get_free_count(void);
uint32_t page_get_reserved_count(void);

// buddy.c

void       buddy_init(void);
void       buddy_print(void);
uintptr_t *buddy_alloc_pages(uint32_t size);
