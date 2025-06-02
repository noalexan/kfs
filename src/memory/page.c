#include "page.h"
#include "printk.h"

// struct page {
//     uint32_t flags;
// #define PAGE_RESERVED       0b00000001
// #define PAGE_BUDDY          0b00000010
// #define PAGE_ALLOCATED      0b00000100
// };

uint32_t reserved_count = 0;
uint32_t free_count     = 0;
page_t  *page_descriptors;

// TODO : add flags when other are implemented
static uint32_t page_get_appropriate_flag(uintptr_t addr_start)
{
	if (boot_allocator_range_overlaps(addr_start, addr_start + PAGE_SIZE, RESERVED_MEMORY) ||
	    boot_allocator_range_overlaps(addr_start, addr_start + PAGE_SIZE, HOLES_MEMORY)) {
		return PAGE_RESERVED;
	}
	return PAGE_BUDDY;
}

static void count_free_pages(page_t *page, void *counter)
{
	if (PAGE_IS_FREE(page))
		(*(uint32_t *)counter)++;
}

static void count_reserved_pages(page_t *page, void *counter)
{
	if (!PAGE_IS_FREE(page))
		(*(uint32_t *)counter)++;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// External Apis

uint32_t page_to_index(page_t *page) { return page - page_descriptors; }

uintptr_t page_to_phys(page_t *page)
{
	uint32_t idx = page_to_index(page);
	return idx * PAGE_SIZE;
}

page_t *page_index_to_page(uint32_t idx)
{
	if (idx >= total_pages)
		return NULL;
	return &page_descriptors[idx];
}

page_t *page_addr_to_page(uintptr_t addr)
{
	return &page_descriptors[ALIGN(addr, PAGE_SIZE) / PAGE_SIZE];
}

bool page_addr_is_same_page(uintptr_t addr1, uintptr_t addr2)
{
	return (addr1 / PAGE_SIZE) == (addr2 / PAGE_SIZE);
}

void page_print_info(page_t *page)
{
	if (!page) {
		printk("page_print_info: NULL page\n");
		return;
	}

	uint32_t  index = page_to_index(page);
	uintptr_t addr  = page_to_phys(page);

	printk("Page: idx=%u addr=0x%x flags=0x%x\n", index, addr, page->flags);

	if (page->flags & PAGE_RESERVED)
		printk("  - RESERVED\n");
	if (page->flags & PAGE_BUDDY)
		printk("  - BUDDY\n");
	if (page->flags & PAGE_ALLOCATED)
		printk("  - ALLOCATED\n");
}

uint32_t page_get_updated_reserved_count(void)
{
	reserved_count = 0;
	page_descriptor_foreach(count_reserved_pages, &reserved_count);
	return reserved_count;
}

uint32_t page_get_updated_free_count(void)
{
	free_count = 0;
	page_descriptor_foreach(count_free_pages, &free_count);
	return free_count;
}

void page_descriptor_foreach(pages_foreach_fn handler, void *data)
{
	for (uint32_t i = 0; i < total_pages; i++)
		handler(&page_descriptors[i], data);
}

uint32_t page_get_free_count(void) { return free_count; }

uint32_t page_get_reserved_count(void) { return reserved_count; }

void page_descriptor_init(void)
{
	page_descriptors = boot_alloc(total_pages * sizeof(page_t));
	for (uint32_t i = 0; i < total_pages; i++) {
		page_descriptors[i].flags        = page_get_appropriate_flag(i * PAGE_SIZE);
		page_descriptors[i].private_data = 0;
	}
}