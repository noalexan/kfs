#include <acpi.h>
#include <kernel/panic.h>
#include <libft.h>
#include <memory/memory.h>
#include <types.h>
#include <utils.h>
#include <x86.h>

////////////////////////////////////////////////
// Header

// Defines

#define END               0x00000000
#define REGION_TYPE_COUNT 3
#define MAX_REGIONS       128

// Macros

#define BOOT_ALLOCATOR_SORT_AND_MERGE(regions, count)                                              \
	do {                                                                                           \
		boot_allocator_sort_regions((regions), (count));                                           \
		(count) = boot_allocator_merge_contiguous_regions((regions), (count));                     \
	} while (0)

// Only in if statement
#define BOOT_ALLOCATOR_IS_REGION_UNMAPPED(start, end)                                              \
	(!(boot_allocator_overlaps((start), (end), RESERVED_MEMORY) ||                                 \
	   boot_allocator_overlaps((start), (end), FREE_MEMORY)))

#define BOOT_ALLOC_FREE_COUNT(alloc)     ((alloc)->count[FREE_MEMORY])
#define BOOT_ALLOC_RESERVED_COUNT(alloc) ((alloc)->count[RESERVED_MEMORY])
#define BOOT_ALLOC_HOLE_COUNT(alloc)     ((alloc)->count[HOLES_MEMORY])

#define BOOT_ALLOC_FREE_REGIONS(alloc)     ((alloc)->regions[FREE_MEMORY])
#define BOOT_ALLOC_RESERVED_REGIONS(alloc) ((alloc)->regions[RESERVED_MEMORY])
#define BOOT_ALLOC_HOLE_REGIONS(alloc)     ((alloc)->regions[HOLES_MEMORY])

// ============================================================================
// STRUCT
// ============================================================================

// Enums

// Structures

typedef struct boot_allocator {
	bool     state;
	uint32_t count[REGION_TYPE_COUNT];
	region_t regions[REGION_TYPE_COUNT][MAX_REGIONS];
} boot_allocator_t;

uint32_t zone_count[MAX_ZONE];
region_t free_zones[MAX_ZONE][MAX_REGIONS];
// Typedefs

typedef void (*regions_foreach_fn)(region_t *regions);

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

extern boot_allocator_t bootmem;
static region_t         all_reg_g[MAX_REGIONS * REGION_TYPE_COUNT];
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
// Code

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
	vga_printf("%p -> %p\n", (void *)reg->start, (void *)reg->end);
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
		vga_printf("No regions to process\n");
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

static void boot_allocator_init_free_zones(void)
{
	size_t    free_count = boot_allocator_get_region_count(FREE_MEMORY);
	region_t *free_reg   = boot_allocator_get_region(FREE_MEMORY);

	for (size_t i = 0; i < free_count; i++) {
		uintptr_t region_start = free_reg[i].start;
		uintptr_t region_end   = free_reg[i].end;

		uintptr_t cur_start = region_start;
		while (cur_start < region_end) {
			int       zone     = -1;
			uintptr_t zone_end = 0;

			if (cur_start >= HIGHMEM_START) {
				zone     = HIGHMEM_ZONE;
				zone_end = HIGHMEM_END;
			} else if (cur_start >= LOWMEM_START) {
				zone     = LOWMEM_ZONE;
				zone_end = HIGHMEM_START;
			} else {
				zone     = DMA_ZONE;
				zone_end = LOWMEM_START;
			}

			uintptr_t sub_end = (region_end < zone_end) ? region_end : zone_end;
			if (sub_end > cur_start) {
				free_zones[zone][zone_count[zone]++] = (region_t){cur_start, sub_end};
			}
			cur_start = sub_end;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
// External APis

region_t *boot_allocator_get_region(enum mem_type type) { return bootmem.regions[type]; }

uint32_t boot_allocator_get_region_count(enum mem_type type) { return bootmem.count[type]; }

region_t *boot_allocator_get_zone(int type) { return free_zones[type]; }

uint32_t boot_allocator_get_zones_count(int type) { return zone_count[type]; }

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
	vga_printf("----------Boot Allocator Printer----------\n");
	vga_printf("Reserved Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_RESERVED_REGIONS(&bootmem),
	                                BOOT_ALLOC_RESERVED_COUNT(&bootmem));
	vga_printf("----------\n");
	vga_printf("Free Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_FREE_REGIONS(&bootmem),
	                                BOOT_ALLOC_FREE_COUNT(&bootmem));
	vga_printf("----------\n");
	vga_printf("Holes Areas : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info,
	                                BOOT_ALLOC_HOLE_REGIONS(&bootmem),
	                                BOOT_ALLOC_HOLE_COUNT(&bootmem));
	vga_printf("------------------------------------------\n");
}

void boot_allocator_zones_printer(void)
{
	vga_printf("----------Boot Allocator Zones Printer----------\n");
	vga_printf("Dma Zone : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info, free_zones[DMA_ZONE],
	                                zone_count[DMA_ZONE]);
	vga_printf("----------\n");
	vga_printf("Lowmem Zone : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info, free_zones[LOWMEM_ZONE],
	                                zone_count[LOWMEM_ZONE]);
	vga_printf("----------\n");
	vga_printf("Highmem Zone : \n");
	boot_allocator_for_each_regions(boot_allocator_print_region_info, free_zones[HIGHMEM_ZONE],
	                                zone_count[HIGHMEM_ZONE]);
	vga_printf("------------------------------------------\n");
}

void boot_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
{
	BOOT_ALLOC_FREE_COUNT(&bootmem)     = 0;
	BOOT_ALLOC_RESERVED_COUNT(&bootmem) = 0;
	BOOT_ALLOC_HOLE_COUNT(&bootmem)     = 0;
	// Zone VGA/BIOS_ROM (0xa0000-0x100000)
	boot_allocator_add_region(&bootmem, 0xa0000, 0x100000, RESERVED_MEMORY);
	// Zone Low memory
	const gdt_ptr_t *gdtr = gdtr_getter();
	const idtr_t    *idtr = idtr_getter();
	boot_allocator_add_region(&bootmem, gdtr->base, (gdtr->base + gdtr->limit + 1),
	                          RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, idtr->base, (idtr->base + idtr->limit + 1),
	                          RESERVED_MEMORY);
	boot_allocator_add_region(&bootmem, 0x0, 0x1000, RESERVED_MEMORY);
	// Zone Inconnu --> todo : understand wtf is this shit, marked as problematic at the moment
	boot_allocator_add_region(&bootmem, 0x1000, 0x9fc00, RESERVED_MEMORY);
	// Zone Kernel Code
	boot_allocator_add_region(&bootmem, (uintptr_t)kernel_start, (uintptr_t)kernel_end,
	                          RESERVED_MEMORY);

	// Zone Boot_info
	boot_allocator_add_region(&bootmem, (uintptr_t)mb2info,
	                          (uintptr_t)(mb2info + mb2info->total_size), RESERVED_MEMORY);
	// boot_allocator_add_region(&bootmem, 0xff00000000, 0xffffffffff, RESERVED_MEMORY);

	mb2_mmap_iter(mmap, mmap_end, boot_allocator_reserved_wrapper, false);
	BOOT_ALLOCATOR_SORT_AND_MERGE(bootmem.regions[RESERVED_MEMORY], bootmem.count[RESERVED_MEMORY]);

	mb2_mmap_iter(mmap, mmap_end, boot_allocator_free_handler, true);
	BOOT_ALLOCATOR_SORT_AND_MERGE(bootmem.regions[FREE_MEMORY], bootmem.count[FREE_MEMORY]);

	boot_allocator_fill_gaps_as_holes();
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_init_total_size, false);
	mb2_mmap_iter(mmap, mmap_end, boot_allocator_init_total_size, true);

	total_pages = boot_allocator_get_total_visibale_ram(&bootmem) / PAGE_SIZE;
	boot_allocator_init_free_zones();
	for (int zone = 0; zone < MAX_ZONE; zone++) {
		BOOT_ALLOCATOR_SORT_AND_MERGE(free_zones[zone], zone_count[zone]);
	}
}

/*
 * TODO : improve this shitty allocator
 * Align Allocation without creating micro fragmentation
 * Deny allocation too small or handle it with more memory as you want bro
 */

void *boot_alloc(uint32_t size)
{
	if (bootmem.state == FROZEN) {
		vga_printf("Error: boot allocator is frozen\n");
		return NULL;
	}

	for (int i = BOOT_ALLOC_FREE_COUNT(&bootmem) - 1; i >= 0; i--) {
		region_t *reg         = &bootmem.regions[FREE_MEMORY][i];
		uint32_t  region_size = reg->end - reg->start;

		if (region_size >= size) {
			void *ret = (void *)reg->start;

			if ((uintptr_t)ret + size > reg->end) {
				vga_printf("ERROR: Allocation would exceed region!\n");
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
	vga_printf("Error: boot allocator: No space left on device\n");
	return NULL;
}
