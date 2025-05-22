#include "map.h"
#include "../gdt.h"
#include "../idt.h"
#include "../panic.h"
#include "../printk/printk.h"
#include "utils.h"
#include <libft.h>

#define UNUSED_MEMORY   0
#define RESERVED_MEMORY 1

uint8_t        *bitmap;
static uint32_t num_of_pages = 0;
static uint32_t bitmap_size  = 0;

used_regions_t used_regions = {0};

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

/*
 *
 * struct multiboot_mmap_entry {
 *     uint64_t addr;   // first phy addr of the range
 *     uint64_t len;    // memory range
 *     uint32_t type;   // range type
 *     uint32_t zero;   // réserved
 * } __attribute__((packed));
 *
 *  Available range type:
 *      1	Available memory RAM
 *      2	Reserved#include <libft.h>

 *
 */

static inline multiboot_memory_map_t *next_entry(multiboot_memory_map_t *mmap_entry,
                                                 multiboot_tag_mmap_t   *mmap)
{
	return (multiboot_memory_map_t *)((uint8_t *)mmap_entry + mmap->entry_size);
}

static inline uint32_t entry_len(multiboot_memory_map_t *entry) { return (uint32_t)entry->len; }

static bool memory_map_register_used_region(uintptr_t start, uintptr_t end)
{
	if (used_regions.count == MAX_REGIONS)
		return false;
	used_regions.regions[used_regions.count] = (region_t){start, end};
	used_regions.count++;
	return true;
}

static void memory_map_init_used_region(void)
{
	used_regions.regions[0] = (region_t){(uintptr_t)kernel_start, (uintptr_t)kernel_end};
	used_regions.regions[1] =
	    (region_t){(uintptr_t)mb2info, (uintptr_t)mb2info + mb2info->total_size};
	used_regions.regions[2] = (region_t){(uintptr_t)gdtr.base, (uintptr_t)(gdtr.limit + 1)};
	used_regions.regions[3] = (region_t){(uintptr_t)idtr.base, (uintptr_t)(idtr.limit + 1)};
	used_regions.count += 4;
	// print_used_regions();
}

uint32_t memory_map_get_total_size(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	uint32_t total_size = 0;

	for (multiboot_memory_map_t *mmap_entry = mmap->entries; (uint8_t *)mmap_entry < mmap_end;
	     mmap_entry                         = next_entry(mmap_entry, mmap)) {
		total_size += (uint32_t)(mmap_entry->len);
	}
	return total_size;
}

static bool overlaps(uintptr_t start, uintptr_t end)
{
	for (size_t i = 0; i < used_regions.count; i++) {
		if (!(end <= used_regions.regions[i].start || start >= used_regions.regions[i].end)) {
			return true;
		}
	}
	return false;
}

static uintptr_t memory_map_get_bitmap_addr(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end,
                                            size_t bitmap_size)
{
	for (multiboot_memory_map_t *entry = mmap->entries; (uint8_t *)entry < mmap_end;
	     entry                         = next_entry(entry, mmap)) {

		if (entry->type != 1)
			continue;

		uintptr_t region_start = ALIGN((uintptr_t)entry->addr, PAGE_SIZE);
		uintptr_t region_end   = (uintptr_t)entry->addr + entry->len;

		for (uintptr_t addr = region_start; addr + bitmap_size <= region_end; addr += PAGE_SIZE) {
			if (!overlaps(addr, addr + bitmap_size)) {
				return addr;
			}
		}
	}
	return 0;
}

static void memory_map_bitmap_fill_addr_range(uint32_t addr_start, uint32_t addr_end,
                                              bool fill_type)
{
	uint32_t page_start = addr_start / PAGE_SIZE;
	uint32_t page_end   = DIV_ROUND_UP(addr_end, PAGE_SIZE);

	for (uint32_t page = page_start; page < page_end; page++) {
		uint32_t byte_index = page / 8;
		uint8_t  bit_index  = page % 8;
		if (fill_type == UNUSED_MEMORY) {
			bitmap[byte_index] &= ~(1 << bit_index);
		} else {
			bitmap[byte_index] |= (1 << bit_index);
		}
	}
}

static void memory_map_fill_bitmap(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end, bool fill_type)
{
	for (multiboot_memory_map_t *entry = mmap->entries; (uint8_t *)entry < mmap_end;
	     entry                         = next_entry(entry, mmap)) {

		if (fill_type == UNUSED_MEMORY && entry->type == 1)
			memory_map_bitmap_fill_addr_range(entry->addr, (entry->addr + entry->len), fill_type);
		else if (fill_type == RESERVED_MEMORY && entry->type != 1)
			memory_map_bitmap_fill_addr_range(entry->addr, (entry->addr + entry->len), fill_type);
	}
}

void memory_map_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	if (!mmap)
		kpanic("memory_init_map: Invalid mmap\n");

	memory_map_init_used_region();

	uint32_t total_size = memory_map_get_total_size(mmap, mmap_end);

	num_of_pages = DIV_ROUND_UP(total_size, PAGE_SIZE);
	bitmap_size  = DIV_ROUND_UP(num_of_pages, 8);

	uintptr_t bitmap_addr = memory_map_get_bitmap_addr(mmap, mmap_end, bitmap_size);
	if (!bitmap_addr)
		kpanic("No space to store the bitmap\n");
	memory_map_register_used_region(bitmap_addr, bitmap_addr + bitmap_size);
	bitmap = (uint8_t *)bitmap_addr;
	ft_memset(bitmap, 0xff, bitmap_size);
	memory_map_fill_bitmap(mmap, mmap_end, UNUSED_MEMORY);
	memory_map_fill_bitmap(mmap, mmap_end, RESERVED_MEMORY);
	for (uint32_t i = 0; i < used_regions.count; i++) {
		memory_map_bitmap_fill_addr_range(used_regions.regions[i].start,
		                                  used_regions.regions[i].end, RESERVED_BIT);
	}
}