#include "page.h"

// struct page {
//     uint32_t flags;
// #define PAGE_RESERVED       0b00000001
// #define PAGE_BUDDY          0b00000010
// #define PAGE_ALLOCATED      0b00000100
// };

page_t *page_descriptors;

// TODO : add flags when other are implemented
static uint32_t page_get_appropriate_flag(uintptr_t addr_start)
{
	if (boot_allocator_range_overlaps(addr_start, addr_start + PAGE_SIZE, RESERVED_MEMORY) ||
	    boot_allocator_range_overlaps(addr_start, addr_start + PAGE_SIZE, HOLES_MEMORY)) {
		return PAGE_RESERVED;
	}
	return PAGE_BUDDY;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// External Apis

/*
 * TODO :
 * Get page by idx
 * get page addr
 * page get index
 * set page allocated
 * set page not allocated
 * page is free
 * page is allocated
 * addr are on same page
 * print page info
 */

page_t *page_get_with_addr(uintptr_t addr)
{
	return &page_descriptors[ALIGN(addr, PAGE_SIZE) / PAGE_SIZE];
}

void page_descriptor_init(void)
{
	page_descriptors = boot_alloc(total_pages * sizeof(page_t));
	page_t *tmp      = page_descriptors;

	for (uint32_t i = 0; i < total_pages; i++) {
		page_descriptors[i].flags        = page_get_appropriate_flag(i * PAGE_SIZE);
		page_descriptors[i].private_data = 0;
	}
}