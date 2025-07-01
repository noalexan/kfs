#pragma once

// ============================================================================
// INCLUDES
// ============================================================================

#include "boot_allocator.h"
#include "buddy.h"
#include "page.h"

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

#define DMA_START 0x00000000
#define DMA_END   0x00FFFFFF // 16 Mo - 1 (0x01000000 - 1)

// Zone NORMAL (lowmem) : 16 Mo -> 896 Mo
#define LOWMEM_START 0x01000000 // 16 Mo
#define LOWMEM_END   0x37FFFFFF // 896 Mo - 1 (0x38000000 - 1)

// Zone HIGHMEM : 896 Mo -> max addr
#define HIGHMEM_START 0x38000000 // 896 Mo
#define HIGHMEM_END   0xFFFFFFFF // max addr

#define MAX_ZONE 3

// Macros

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

// Typedefs

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

// (aucune variable globale à exposer ici)

// ============================================================================
// EXTERNAL APIs
// ============================================================================

// API mémoire globale (à adapter selon ton implémentation)
// void *kmalloc(size_t size);
// void  kfree(void *ptr);
// void *krealloc(void *ptr, size_t new_size);
