#pragma once

#include "../mb2_info.h"
#include <multiboot2.h>

#define MAX_REGIONS 32

typedef struct region_s {
	uintptr_t start;
	uintptr_t end;
} region_t;

typedef struct used_regions_s {
	uint32_t count;
	region_t regions[MAX_REGIONS];
} used_regions_t;

typedef struct multiboot_tag_mmap multiboot_tag_mmap_t;

extern uint8_t *bitmap;

void memory_map_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end);