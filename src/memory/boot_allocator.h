#pragma once

#include "../mb2_info.h"
#include <multiboot2.h>
#include <types.h>

#define MAX_REGIONS 256
#define END         0x00000000

enum mem_type { FREE_MEMORY = 0, RESERVED_MEMORY };

typedef struct region_s {
	uintptr_t start;
	uintptr_t end;
} region_t;

typedef struct boot_allocator {
	uint32_t free_count;
	region_t free_regions[MAX_REGIONS];
	uint32_t reserved_count;
	region_t reserved_regions[MAX_REGIONS];
} boot_allocator_t;

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);