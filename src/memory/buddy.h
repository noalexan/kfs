#pragma once

// ============================================================================
// IMCLUDES
// ============================================================================

#include <memory.h>

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

// Typedefs

typedef struct buddy_allocator buddy_allocator_t;

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// Internal APIs
// ============================================================================

// Statics

static struct list_head *page_to_list_node(page_t *page);
static page_t           *list_node_to_page(struct list_head *node);
static void              buddy_list_add_head(struct list_head *new_node, struct list_head *head);
static struct list_head *order_to_free_list(uint32_t order);
static uint32_t          order_to_nrFree(uint32_t order);
static uint32_t          size_to_order(uint32_t size);
static struct list_head *pop_node(struct list_head *node);
static uintptr_t        *pop_first_block(uint32_t order);
static void              set_block_metadata(uintptr_t *ret, uint32_t order, int type);
static uintptr_t        *alloc_block_with_order(uint32_t order);
static uintptr_t *split_block_to_order(uint32_t order_needed, uint32_t cur_order, uintptr_t *ret);

// Debugs

static const char *debug_buddy_order_to_string(int order);
static void        print_buddy_free_list(size_t order);
static void        debug_buddy_corrupted_list(size_t order);
static void        debug_buddy_alloc_still_free(size_t order, uintptr_t *phys);
static void        debug_buddy_simple_alloc(void);
static void        debug_buddy_check_lost_pages(void);
static void        debug_buddy_panic(const char *func);
static void        debug_buddy_print_node_info(struct list_head *node);