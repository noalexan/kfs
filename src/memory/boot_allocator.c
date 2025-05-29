#include "boot_allocator.h"
#include "gdt.h"
#include "idt.h"
#include "panic.h"
#include <libft.h>
#include <memory.h>

/*
 *  The following APIs are used to store precoce allocations and to keep a track of evry used memory
 *  before the memory system is available
 */

// Globals

extern uint8_t   kernel_start[];
extern uint8_t   kernel_end[];
boot_allocator_t bootmem;

////////////////////////////////////////////////
// Need refactor

// APIs

/*
 * Adds a memory region (reserved or free) to the boot allocator's region tables
 * Panics if the region is invalid or if the maximum number of regions is exceeded
 */

static void boot_allocator_add_region(boot_allocator_t *alloc, uintptr_t start, uintptr_t end,
                                      enum mem_type type)
{
	char *str_type = (type == RESERVED_MEMORY ? "reserved" : "free");
	if (start > end && end != END) {
		kpanic("Error: boot allocator - start: %p end: %p address is greater than end address\n",
		       start, end);
	} else if (start == end)
		return;
	if (type == RESERVED_MEMORY) {
		if (alloc->reserved_count >= MAX_REGIONS)
			kpanic("Error: boot allocator too many %s regions registered\n", str_type);
		alloc->reserved_regions[alloc->reserved_count++] = (region_t){start, end};
		return;
	} else if (type == FREE_MEMORY) {
		if (alloc->free_count >= MAX_REGIONS)
			kpanic("Error: boot allocator too many %s regions registered\n", str_type);
		alloc->free_regions[alloc->free_count++] = (region_t){start, end};
		return;
	}
	kpanic("Error: boot_alloc: add_region fun unknown memory type\n");
}

/*
 * Wrapper function for adding a reserved region to the boot allocator
 * Used as a callback/handler for memory map iteration
 */

void boot_allocator_reserved_wrapper(uintptr_t start, uintptr_t end)
{
	boot_allocator_add_region(&bootmem, start, end, RESERVED_MEMORY);
}

/*
 * Checks if a given address range overlaps with any region of the specified memory type
 */

static bool boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type)
{
	uint32_t  count = (type == RESERVED_MEMORY ? bootmem.reserved_count : bootmem.free_count);
	region_t *reg   = (type == RESERVED_MEMORY ? bootmem.reserved_regions : bootmem.free_regions);
	for (size_t i = 0; i < count; i++) {
		if (!(end <= reg[i].start || start >= reg[i].end)) {
			return true;
		}
	}
	return false;
}

/*
 * Checks if a given address overlaps
 */

static bool boot_allocator_addr_overlaps(uint32_t addr)
{
	for (uint32_t i = 0; i < bootmem.reserved_count; i++) {
		region_t res = bootmem.reserved_regions[i];
		if (addr >= res.start && addr < res.end) {
			return true;
		}
	}
	for (uint32_t i = 0; i < bootmem.free_count; i++) {
		region_t fr = bootmem.free_regions[i];
		if (addr >= fr.start && addr < fr.end) {
			return true;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////

static void boot_allocator_sort_regions(region_t *reg, uint32_t count)
{
	for (uint32_t i = 1; i < count; i++) {
		region_t current = reg[i];
		uint32_t j       = i + 1;

		while (--j > 0 && reg[j - 1].start > current.start)
			reg[j] = reg[j - 1];
		reg[j] = current;
	}
}

static void boot_allocator_free_handler(uintptr_t start, uintptr_t end)
{
	uintptr_t cur = start;
	printk("Free area : s %p | e %p\n", start, end);
	for (uint32_t i = 0; i < bootmem.reserved_count; i++) {
		region_t res = bootmem.reserved_regions[i];

		if (res.end <= cur || res.start >= end)
			continue;
		if (cur < res.start && !boot_allocator_range_overlaps(cur, res.start, FREE_MEMORY)) {
			boot_allocator_add_region(&bootmem, cur, res.start, FREE_MEMORY);
		}
		if (res.end > cur)
			cur = res.end;
		if (cur >= end)
			return;
	}
	if (cur < end && !boot_allocator_range_overlaps(cur, end, FREE_MEMORY)) {
		boot_allocator_add_region(&bootmem, cur, end, FREE_MEMORY);
	}
}

static uint32_t boot_allocator_merge_contiguous_regions(region_t *reg, uint32_t count)
{
	region_t cur          = *reg;
	uint32_t merged_count = 0;
	region_t merged[MAX_REGIONS];

	if (!count)
		return count;
	ft_bzero(merged, sizeof(region_t) * MAX_REGIONS);
	boot_allocator_sort_regions(reg, count);
	for (uint32_t i = 1; i < count; i++) {
		if (cur.end == reg[i].start) {
			cur.end = reg[i].end;
		} else {
			merged[merged_count++] = cur;
			cur                    = reg[i];
		}
	}
	merged[merged_count++] = cur;
	ft_memcpy(reg, merged, sizeof(region_t) * MAX_REGIONS);
	return merged_count;
}

/*
 * Tools for debug :
 * boot_allocator_print_region_info
 * boot_allocator_for_each_regions
 * boot_allocator_printer
 */

static void boot_allocator_print_region_info(region_t *reg)
{
	printk("%p -> %p\n", (void *)reg->start, (void *)reg->end);
}

static void boot_allocator_for_each_regions(regions_foreach_fn handler, region_t *reg,
                                            uint32_t count)
{
	for (uint32_t i = 0; i < count; i++) {
		handler(&reg[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// External APis

void boot_allocator_printer(void)
{
	printk("----------Boot Allocator Printer----------\n");
	printk("Reserved Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info, bootmem.reserved_regions,
	                                bootmem.reserved_count);
	printk("Free Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info, bootmem.free_regions,
	                                bootmem.free_count);
	printk("------------------------------------------\n");
}

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	bootmem.free_count     = 0;
	bootmem.reserved_count = 0;
	boot_allocator_add_region(&bootmem, kernel_start, kernel_end, RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, gdtr.base, (gdtr.base + gdtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, idtr.base, (idtr.base + idtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, mb2info, (mb2info + mb2info->total_size), RESERVED_MEMORY);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, false);
	boot_allocator_sort_regions(bootmem.reserved_regions, bootmem.reserved_count);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_free_handler, true);
	bootmem.free_count =
	    boot_allocator_merge_contiguous_regions(bootmem.free_regions, bootmem.free_count);
	bootmem.reserved_count =
	    boot_allocator_merge_contiguous_regions(bootmem.reserved_regions, bootmem.reserved_count);
}

/*
 * TODO : improve this shitty allocator
 * Align Allocation without creating micro fragmentation
 * Deny allocation too small or handle it with more memory as you want bro
 */

void *boot_alloc(uint32_t size)
{
	if (bootmem.state == FROZEN) {
		printk("Error: boot allocator is frozen\n");
		return NULL;
	}

	for (int i = bootmem.free_count - 1; i >= 0; i--) {
		region_t *reg         = &bootmem.free_regions[i];
		uint32_t  region_size = reg->end - reg->start;

		if (region_size >= size) {
			void *ret = (void *)reg->start;
			reg->start += size;

			if (reg->start == reg->end) {
				for (int j = i; j < bootmem.free_count - 1; j++)
					bootmem.free_regions[j] = bootmem.free_regions[j + 1];
				bootmem.free_count -= 1;
			}

			boot_allocator_add_region(&bootmem, (uintptr_t)ret, (uintptr_t)ret + size,
			                          RESERVED_MEMORY);
			return ret;
		}
	}
	printk("Error: boot allocator: No space left on device\n");
	return NULL;
}