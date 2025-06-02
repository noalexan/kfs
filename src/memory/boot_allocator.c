#include "boot_allocator.h"
#include "gdt.h"
#include "idt.h"
#include "panic.h"
#include <libft.h>
#include <utils.h>

/*
 *  The following APIs are used to store precoce allocations and to keep a track of evry used memory
 *  before the memory system is available
 */

// Globals

uint32_t         total_pages = 0;
uint32_t         total_RAM   = 0;
boot_allocator_t bootmem;
extern uint8_t   kernel_start[];
extern uint8_t   kernel_end[];

////////////////////////////////////////////////

// APIs

/*
 * Adds a memory region (reserved or free) to the boot allocator's region tables
 * Panics if the region is invalid or if the maximum number of regions is exceeded
 */

static void boot_allocator_add_region(boot_allocator_t *alloc, uintptr_t start, uintptr_t end,
                                      enum mem_type type)
{
	if (start > end && end != 0) {
		kpanic("boot_alloc: %s: Invalid region start=%p end=%p\n", __func__, start, end);
	} else if (start == end) {
		return;
	} else if (type >= REGION_TYPE_COUNT)
		kpanic("boot_alloc: %s: Unknown memory type\n", __func__);

	if (alloc->count[type] >= MAX_REGIONS) {
		kpanic("boot_alloc: %s: too many %s regions\n", __func__);
	}

	uint32_t index              = alloc->count[type];
	alloc->regions[type][index] = (region_t){start, end};
	alloc->count[type]++;
}

/*
 * Wrapper function for adding a reserved region to the boot allocator
 * Used as a callback/handler for memory map iteration
 */

void boot_allocator_reserved_wrapper(uintptr_t start, uintptr_t end)
{
	boot_allocator_add_region(&bootmem, start, end, RESERVED_MEMORY);
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
	for (uint32_t i = 0; i < BOOT_ALLOC_RESERVED_COUNT(&bootmem); i++) {
		region_t res = BOOT_ALLOC_RESERVED_REGIONS(&bootmem)[i];

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

static region_t *boot_allocator_get_all_regions(boot_allocator_t *alloc)
{
	uint32_t idx = 0;

	for (uint32_t type = 0; type < REGION_TYPE_COUNT; type++) {
		for (uint32_t i = 0; i < alloc->count[type]; i++) {
			ft_memcpy(&all_reg_g[idx++], &alloc->regions[type][i], sizeof(region_t));
		}
	}
	return all_reg_g;
}

static void boot_allocator_fill_gaps_as_holes(void)
{
	uint32_t total_reg = BOOT_ALLOC_RESERVED_COUNT(&bootmem) + BOOT_ALLOC_FREE_COUNT(&bootmem);

	if (total_reg == 0) {
		printk("No regions to process\n");
		return;
	}

	region_t *all_reg = boot_allocator_get_all_regions(&bootmem);

	BOOT_ALLOCATOR_SORT_AND_MERGE(all_reg, total_reg);

	for (uint32_t i = 0; i < total_reg - 1; i++) {
		uintptr_t gap_start = all_reg[i].end;
		uintptr_t gap_end   = all_reg[i + 1].start;
		if (gap_start < gap_end)
			boot_allocator_add_region(&bootmem, gap_start, gap_end, HOLES_MEMORY);
	}

	BOOT_ALLOCATOR_SORT_AND_MERGE(bootmem.regions[HOLES_MEMORY], bootmem.count[HOLES_MEMORY]);
}

static void boot_allocator_init_total_size(uintptr_t start, uintptr_t end)
{
	if ((uint32_t)end == 0)
		end = 0xffffffff;
	total_RAM += end - start;
}

static uint32_t boot_allocator_get_total_visibale_ram(boot_allocator_t *alloc)
{
	uint32_t total_count = BOOT_ALLOC_FREE_COUNT(alloc) + BOOT_ALLOC_RESERVED_COUNT(alloc) +
	                       BOOT_ALLOC_HOLE_COUNT(alloc);

	if (total_count <= 0) {
		kpanic("Error: %s: this function cannot be called before memory parsing\n", __func__);
	}

	region_t *all_reg = boot_allocator_get_all_regions(alloc);
	BOOT_ALLOCATOR_SORT_AND_MERGE(all_reg, total_count);
	uintptr_t end = all_reg[total_count - 1].end;
	if (end == 0)
		return 0xffffffff;
	return end;
}

//////////////////////////////////////////////////////////////////////////////////////////
// External APis

region_t *boot_allocator_get_region(enum mem_type type) { return bootmem.regions[type]; }

uint32_t boot_allocator_get_region_count(enum mem_type type) { return bootmem.count[type]; }

void boot_allocator_freeze(void) { bootmem.state = FROZEN; }

/*
 * Checks if a given address range overlaps with any region of the specified memory type
 */

bool boot_allocator_range_overlaps(uintptr_t start, uintptr_t end, enum mem_type type)
{
	uint32_t  count   = bootmem.count[type];
	region_t *regions = bootmem.regions[type];

	for (uint32_t i = 0; i < count; i++) {
		if (!(end <= regions[i].start || start >= regions[i].end)) {
			return true;
		}
	}
	return false;
}

void boot_allocator_printer(void)
{
	printk("----------Boot Allocator Printer----------\n");
	printk("Reserved Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_RESERVED_REGIONS(&bootmem),
	                                BOOT_ALLOC_RESERVED_COUNT(&bootmem));
	printk("----------\n");
	printk("Free Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_FREE_REGIONS(&bootmem),
	                                BOOT_ALLOC_FREE_COUNT(&bootmem));
	printk("----------\n");
	printk("Holes Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_HOLE_REGIONS(&bootmem),
	                                BOOT_ALLOC_HOLE_COUNT(&bootmem));
	printk("------------------------------------------\n");
}

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	BOOT_ALLOC_FREE_COUNT(&bootmem)     = 0;
	BOOT_ALLOC_RESERVED_COUNT(&bootmem) = 0;
	BOOT_ALLOC_HOLE_COUNT(&bootmem)     = 0;
	boot_allocator_add_region(&bootmem, (uintptr_t)kernel_start, (uintptr_t)kernel_end,
	                          RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, gdtr.base, (gdtr.base + gdtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, idtr.base, (idtr.base + idtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, (uintptr_t)mb2info,
	                          (uintptr_t)(mb2info + mb2info->total_size), RESERVED_MEMORY);

	mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, false);
	BOOT_ALLOCATOR_SORT_AND_MERGE(bootmem.regions[RESERVED_MEMORY], bootmem.count[RESERVED_MEMORY]);

	mb2_mmap_iter(mmap, mmap_end, boot_allocator_free_handler, true);
	BOOT_ALLOCATOR_SORT_AND_MERGE(bootmem.regions[FREE_MEMORY], bootmem.count[FREE_MEMORY]);

	boot_allocator_fill_gaps_as_holes();
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_init_total_size, false);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_init_total_size, true);

	total_pages = boot_allocator_get_total_visibale_ram(&bootmem) / PAGE_SIZE;
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

	for (int i = BOOT_ALLOC_FREE_COUNT(&bootmem) - 1; i >= 0; i--) {
		region_t *reg         = &bootmem.regions[FREE_MEMORY][i];
		uint32_t  region_size = reg->end - reg->start;

		if (region_size >= size) {
			void *ret = (void *)reg->start;

			if ((uintptr_t)ret + size > reg->end) {
				printk("ERROR: Allocation would exceed region!\n");
				continue;
			}

			reg->start += size;

			boot_allocator_add_region(&bootmem, (uintptr_t)ret, (uintptr_t)ret + size,
			                          RESERVED_MEMORY);

			if (reg->start >= reg->end) {
				*reg = bootmem.regions[FREE_MEMORY][--BOOT_ALLOC_FREE_COUNT(&bootmem)];
			}

			return ret;
		}
	}
	printk("Error: boot allocator: No space left on device\n");
	return NULL;
}
