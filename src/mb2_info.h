#pragma once

#include <multiboot2.h>
#include <types.h>

typedef struct multiboot_tag_mmap multiboot_tag_mmap_t;

typedef void (*entry_handler_t)(uintptr_t start, uintptr_t end);

/* Struct for mb2 parsing*/

typedef struct multiboot_info {
	uint32_t             total_size;
	uint32_t             reserved;
	struct multiboot_tag tags[0];
} multiboot_info_t;

struct multiboot2_header_tag_end {
	uint16_t type;
	uint16_t flags;
	uint32_t size;
} __attribute__((packed, aligned(8)));

/*  Struct used to handle all requested tags */

struct multiboot2_header_tag_information_request {
	uint16_t type;
	uint16_t flags;
	uint32_t size;
	uint32_t requests[1];
} __attribute__((packed, aligned(8)));

extern multiboot_info_t *mb2info;

void mb2_mmap_iter(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end, entry_handler_t handler,
                   bool free);
void mb2_init(uint32_t magic, uint32_t mbi_addr);
