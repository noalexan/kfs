#pragma once

#define PAGE_SIZE 4096

#define get_page_status(page_idx) ((bitmap[(page_idx) / 8] >> ((page_idx) % 8)) & 1)
#define get_page(page_idx) ((page_table_entry *)((page_idx) * PAGE_SIZE))

enum page_flags {
	PDE_PRESENT       = 0x01,
	PDE_WRITABLE      = 0x02,

	PDE_KERNEL        = 0x00,
	PDE_USER          = 0x04,

	PDE_WRITE_THROUGH = 0x08,
	PDE_CACHE_DISABLE = 0x10,
	PDE_ACCESSED      = 0x20,
	PDE_RESERVED      = 0x40,
	PDE_PAGE_SIZE     = 0x80,
	PDE_GLOBAL        = 0x100
};

typedef uint32_t page_directory_entry;
typedef uint32_t page_table_entry;

void paging_init(void);

page_table_entry *alloc_page();
void free_page(page_table_entry *page);
