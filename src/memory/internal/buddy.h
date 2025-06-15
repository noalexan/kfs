#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

#include <acpi.h>
#include <drivers/vga.h>
#include <kernel/panic.h>
#include <libft.h>
#include <memory/boot_allocator.h>
#include <memory/buddy.h>
#include <utils.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define MAX_ORDER 10

// Macros

#define WHO_IS_MY_BUDDY(addr, order, base) ((((addr) - (base)) ^ ORDER_TO_BYTES(order)) + (base))

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

struct list_head {
	struct list_head *next, *prev;
};

struct buddy_free_area {
	struct list_head free_list[MAX_MIGRATION];
	uint32_t         nr_free;
};

struct buddy_allocator {
	struct buddy_free_area areas[MAX_ORDER + 1];
};

// Typedefs

typedef struct buddy_allocator buddy_allocator_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

static buddy_allocator_t buddy[MAX_ZONE];

// ============================================================================
// Internal APIs
// ============================================================================

// Statics

static bool       order_is_valid(int order);
static size_t     order_to_nrFree(size_t order);
static size_t     size_to_order(size_t size);
static page_t    *list_node_to_page(struct list_head *node);
static uintptr_t  get_buddy_base(uintptr_t addr);
static uintptr_t *pop_first_block(size_t order);
static uintptr_t *alloc_block_with_order(size_t order);
static uintptr_t *split_block_to_order(size_t order_needed, size_t cur_order, uintptr_t *ret);
static void       set_block_metadata(uintptr_t *ret, size_t order, int type);
static void       buddy_list_add_head(struct list_head *new_node, struct list_head *head);
static struct list_head *ptr_to_list(void *ptr);
static struct list_head *page_to_list_node(page_t *page);
static struct list_head *order_to_free_list(size_t order);
static struct list_head *pop_node(struct list_head *node);
static struct list_head *get_buddy_node(void *block, size_t order);

// Debugs

static const char *debug_buddy_order_to_string(int order);
static void        print_buddy_free_list(size_t order);
static void        debug_buddy_print_node_info(struct list_head *node);
static void        debug_buddy_corrupted_list(size_t order);
static void        debug_buddy_panic(const char *func);
static void        debug_buddy_check_lost_pages(void);
static void        debug_buddy_alloc_still_free(size_t order, uintptr_t *phys);
static void        buddy_drain_lower_orders(void);
static void        debug_buddy_simple_alloc(void);
static void        debug_buddy_check_all_list_corrumption(void);
static void        debug_buddy_split_block(void);
static void        debug_buddy_free_block(void);

void debug_buddy(void);
