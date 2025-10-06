#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define MAX_SLAB_SIZE 2048

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

typedef struct slab_cache {
	struct list_head slabs_full;
	struct list_head slabs_partial;
	struct list_head slabs_empty;
	size_t           object_size;
	uint32_t         objects_per_slab;

} slab_cache_t;

typedef struct slab {
	struct list_head list;
	void            *freelist;
	uint32_t         inuse;
	slab_cache_t    *parent_cache;
	char             _padding[12];
} slab_t;

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// ============================================================================
// EXTERNAL APIs
// ============================================================================

void  slab_init(void);
void  slab_free(void *ptr);
void *slab_alloc(size_t size, zone_type zone);
void  slab_print_summary(void);
void  slab_shrink_caches(zone_type zone);
