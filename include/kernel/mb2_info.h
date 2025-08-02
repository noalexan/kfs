#pragma once

#include <multiboot2.h>
#include <types.h>

typedef struct multiboot_tag_mmap multiboot_tag_mmap_t;
typedef void (*entry_handler_t)(uintptr_t start, uintptr_t end);

typedef struct multiboot_info {
	uint32_t             total_size;
	uint32_t             reserved;
	struct multiboot_tag tags[0];
} multiboot_info_t;

extern multiboot_info_t *mb2info;

void mb2_mmap_iter(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end, entry_handler_t handler,
                   bool free);
void mb2_init(void);
