#pragma once

#include <acpi.h>
#include <kernel/mb2_info.h>
#include <kernel/panic.h>
#include <memory/memory.h>
#include <types.h>
#include <utils.h>

#define TAGS_NEEDED 1

struct multiboot2_header_tag_end {
	uint16_t type;
	uint16_t flags;
	uint32_t size;
} __attribute__((packed, aligned(8)));

struct multiboot2_header_tag_information_request {
	uint16_t type;
	uint16_t flags;
	uint32_t size;
	uint32_t requests[1];
} __attribute__((packed, aligned(8)));

extern const struct multiboot_header                          mb2_header;
extern const struct multiboot2_header_tag_information_request mb2_tag_info_req;
extern const struct multiboot2_header_tag_end                 mb2_tag_end;

static inline multiboot_memory_map_t *next_entry(multiboot_memory_map_t *mmap_entry,
                                                 multiboot_tag_mmap_t   *mmap)
{
	return (multiboot_memory_map_t *)((uint8_t *)mmap_entry + mmap->entry_size);
}
