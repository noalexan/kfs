#include "boot_allocator.h"
#include "../gdt.h"
#include "../idt.h"
#include "../panic.h"

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

static bool boot_allocator_overlaps(uintptr_t start, uintptr_t end, enum mem_type type)
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

static bool boot_allocator_is_region_unmapped(uintptr_t start, uintptr_t end)
{
	if (boot_allocator_overlaps(start, end, RESERVED_MEMORY))
		return false;
	else if (boot_allocator_overlaps(start, end, FREE_MEMORY))
		return false;
	return true;
}

void boot_allocator_print_regions(void)
{
	printk("=== Boot Allocator Regions ===\n");
	for (uint32_t i = 0; i < bootmem.reserved_count; i++) {
		region_t reg = bootmem.reserved_regions[i];
		printk("[RESERVED] %p -> %p\n", (void *)reg.start, (void *)reg.end);
	}
	for (uint32_t i = 0; i < bootmem.free_count; i++) {
		region_t reg = bootmem.free_regions[i];
		printk("[FREE] %p -> %p\n", (void *)reg.start, (void *)reg.end);
	}
}

bool boot_allocator_addr_is_overlapsed(uint32_t addr)
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

void boot_allocator_sort_regions(region_t *reg, uint32_t count)
{
	for (uint32_t i = 1; i < count; i++) {
		region_t current = reg[i];
		uint32_t j       = i + 1;

		while (--j > 0 && reg[j - 1].start > current.start)
			reg[j] = reg[j - 1];
		reg[j] = current;
	}
}

void boot_allocator_free_handler(uintptr_t start, uintptr_t end)
{
	uintptr_t cur = start;
	printk("Free Call s= %p e= %p\n", start, end);
	for (uint32_t i = 0; i < bootmem.reserved_count; i++) {
		region_t res = bootmem.reserved_regions[i];

		if (res.end <= cur || res.start >= end)
			continue;
		if (cur < res.start && !boot_allocator_overlaps(cur, res.start, FREE_MEMORY)) {
			boot_allocator_add_region(&bootmem, cur, res.start, FREE_MEMORY);
		}
		if (res.end > cur)
			cur = res.end;
		if (cur >= end)
			return;
	}
	if (cur < end && !boot_allocator_overlaps(cur, end, FREE_MEMORY)) {
		boot_allocator_add_region(&bootmem, cur, end, FREE_MEMORY);
	}
}

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	boot_allocator_add_region(&bootmem, kernel_start, kernel_end, RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, gdtr.base, (gdtr.base + gdtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, idtr.base, (idtr.base + idtr.limit + 1), RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, mb2info, (mb2info + mb2info->total_size), RESERVED_MEMORY);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, false);
	boot_allocator_sort_regions(bootmem.reserved_regions, bootmem.reserved_count);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_free_handler, true);
	boot_allocator_print_regions();
}

// TODO: refactor region used with iter_on_region(handler, region) + Macro for use on both region
// TODO: sort and merge zones who (start) = (prev end)
// TODO: create an simple function for precoce allocation

/*
 * TODO: after memory
 *****************************
 * create an debugging framwork
 * create an line editing driver
 * Clean up the code
 * Use dynamic memory allocation where is needed (keyboard for example)
 */
