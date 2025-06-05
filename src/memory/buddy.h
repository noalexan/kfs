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
static uintptr_t        *pop_block_with_order(uint32_t order);
static uintptr_t        *split_blocks(uint32_t order_needed, uint32_t cur_order, uintptr_t *ret);

// Debugs