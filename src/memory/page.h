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

typedef struct page page_t;
typedef void (*pages_foreach_fn)(page_t *page, void *data);

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// INTERNAL APIs
// ============================================================================

static inline page_t *last_page(void);
static inline page_t *first_page(void);
static uint32_t       page_get_appropriate_flag(uintptr_t addr_start);
static void           count_free_pages(page_t *page, void *counter);
static void           count_reserved_pages(page_t *page, void *counter);
static void           page_descriptor_foreach(pages_foreach_fn handler, void *data);
