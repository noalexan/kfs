#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include <kernel/mb2_info.h>
#include <types.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define TO_FREE false
#define TO_KEEP true

#define FREE      0
#define ALLOCATED 1

#define KiB_SIZE (1UL << 10)
#define MiB_SIZE (1UL << 20)
#define GiB_SIZE (1UL << 30)

#define MAX_PAGES (1UL << 20)

/* Zones and migrations types */

#define DMA_START 0x00000000
#define DMA_END   0x00FFFFFF // 16 Mo - 1 (0x01000000 - 1)
// Zone NORMAL (lowmem) : 16 Mo -> 896 Mo
#define LOWMEM_START 0x01000000 // 16 Mo
#define LOWMEM_END   0x37FFFFFF // 896 Mo - 1 (0x38000000 - 1)
// Zone HIGHMEM : 896 Mo -> max addr
#define HIGHMEM_START 0x38000000 // 896 Mo
#define HIGHMEM_END   0xFFFFFFFF // max addr

typedef enum { LOWMEM_ZONE = 0, DMA_ZONE, HIGHMEM_ZONE, INVALID_ZONE } zone_type;

#define MAX_ZONE 3

typedef enum {
	MIGRATE_MOVABLE = 0,
	// Not Implemented
	MIGRATE_UNMOVABLE,
	MIGRATE_RECLAIMABLE,
} migration_type;

#define MAX_MIGRATION 1

/* Page tools */
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

#define KERNEL_VADDR_BASE 0xC0000000

// Macros
#define PHYS_TO_VIRT_LINEAR(p_addr) ((void *)((uintptr_t)(p_addr) + KERNEL_VADDR_BASE))
#define VIRT_TO_PHYS_LINEAR(v_addr) ((uintptr_t)((v_addr) - KERNEL_VADDR_BASE))

#define PAGE_BY_ORDER(order)     (1 << order)
#define PAGE_DATA_IS_MAGIC(page) ((page)->private_data == PAGE_MAGIC)
#define PAGE_GET_ZONE(page)      ((page)->flags & PAGE_ZONE_MASK)
#define PAGE_IS_FREE(page)                                                                         \
	(FLAG_IS_SET((page)->flags, PAGE_BUDDY) && !FLAG_IS_SET((page)->flags, PAGE_ALLOCATED))
#define PAGE_IS_UNUSABLE(page)                                                                     \
	(FLAG_IS_SET((page)->flags, PAGE_ALLOCATED) || FLAG_IS_SET((page)->flags, PAGE_RESERVED))

// ============================================================================
// STRUCT
// ============================================================================

// Enums

enum allocator_state { ACTIVE = 0, FROZEN };
enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY, HOLES_MEMORY };

// Structures

struct list_head {
	struct list_head *next, *prev;
};

struct page {
	struct list_head node;
	uint32_t         flags;
	uintptr_t        private_data;
};

struct region_s {
	uintptr_t start;
	uintptr_t end;
};

struct boot_allocator;

// Typedefs

typedef struct region_s       region_t;
typedef struct boot_allocator boot_allocator_t;
typedef struct page           page_t;
typedef enum { NEXT = 0, PREV } direction;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern uint32_t total_pages;
extern uint32_t total_RAM;
extern page_t  *page_descriptors;

// ============================================================================
// EXTERNAL APIs
// ============================================================================

// API mémoire globale (à adapter selon ton implémentation)
// void *kmalloc(size_t size);
// void  kfree(void *ptr);
// void *krealloc(void *ptr, size_t new_size);

uintptr_t *buddy_alloc_pages(size_t size, zone_type zone);
void       buddy_print(zone_type zone);
void       buddy_free_block(void *ptr);
void       buddy_init(void);
void       debug_buddy(void);

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

uint32_t  boot_allocator_get_free_zones_count(int type);
uint32_t  boot_allocator_get_region_count(enum mem_type type);
region_t *boot_allocator_get_free_zone(int type);
region_t *boot_allocator_get_region(enum mem_type type);
void     *boot_alloc(uint32_t size, zone_type zone, bool freeable);
void *boot_alloc_at(uint32_t size, zone_type zone, bool freeable, uintptr_t start, uintptr_t end);
bool  boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type);
void  boot_allocator_freeze(void);
void  boot_allocator_print_inital_layout(void);
void  boot_allocator_res_zones_printer(void);
void  boot_allocator_free_zones_printer(void);
void  boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);
void  boot_alloc_clean_up(void);
void  boot_allocator_print_allocations(void);

void map_page(uint32_t *page_dir, uintptr_t v_addr, uintptr_t p_addr, uint32_t flags);
void paging_init(void);
void page_fault_handler(void);
void paging_cleanup(void);
