#pragma once

#include "../mb2_info.h"
#include "boot_allocator.h"
#include "memory.h"
#include <multiboot2.h>

#define MAX_ORDER 11
#define PAGE_SIZE 4096

struct list_head {
	struct list_head *next, *prev;
};

struct buddy_free_area {
	struct list_head free_list;
	uint32_t         nr_free;
};

typedef struct buddy_free_area buddy_allocator_t;

void buddy_allocator_init(void);