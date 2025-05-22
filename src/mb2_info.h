#pragma once

#include "panic.h"
#include <multiboot2.h>
#include <types.h>
#include <utils.h>

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

void *mb2_get_tag(uint32_t tag);
void  mb2_init(uint32_t magic, uint32_t mbi_addr);