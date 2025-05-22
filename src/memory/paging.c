#include <types.h>

#include "paging.h"

__attribute__((aligned(PAGE_SIZE)))
page_directory_entry *page_directory;

__attribute__((aligned(PAGE_SIZE)))
page_table_entry *page_table;

static inline void claim_page(int page_idx) {
    uint32_t byte_index = page_idx / 8;
    uint8_t bit_index = page_idx % 8;
    bitmap[byte_index] |= (1 << bit_index);
}

static inline void unclaim_page(int page_idx) {
    uint32_t byte_index = page_idx / 8;
    uint8_t bit_index = page_idx % 8;
    bitmap[byte_index] &= ~(1 << bit_index);
}

static inline void enable_paging(void)
{
	// Enable Paging by setting the PG bit in cr0
	__asm__ volatile("mov %%cr0, %%eax\n\t"
	                 "or $0x80000000, %%eax\n\t"
	                 "mov %%eax, %%cr0\n\t"
	                 :
	                 :
	                 : "eax");
}

void paging_init(void)
{
	page_directory = alloc_page();
	page_table = alloc_page();

	// Fill directory by empty entries
	for (size_t i = 0; i < 1024; i++) {
		page_directory[i] = PDE_KERNEL | PDE_WRITABLE;
	}

	// Fill table by empty entries
	for (size_t i = 0; i < 1024; i++) {
		page_table[i] = ((uintptr_t)get_page(i) & 0xfffff000) | PDE_KERNEL | PDE_WRITABLE | PDE_PRESENT;
	}

	// Set first directory entry to page table
	page_directory[0] = ((uintptr_t)page_table & 0xfffff000) | PDE_WRITABLE | PDE_PRESENT;

	// Set page directory pointer to cr3
	__asm__ volatile("mov %0, %%cr3\n\t" : : "r"(page_directory));

	enable_paging();
}

page_table_entry *alloc_page()
{
	int page_idx = 0;

	// Search for bit set to UNUSED_MEMORY in memory bitmap
	while (get_page_status(page_idx) == RESERVED_MEMORY) {
		page_idx++;
	}

	// Then set it to RESERVED_MEMORY
	claim_page(page_idx);

	// Return the address corresponding to the allocated page
	return get_page(page_idx);
}

void free_page(page_table_entry *page) {
	unclaim_page((uintptr_t)page / PAGE_SIZE);
}
