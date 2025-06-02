#include "buddy.h"
#include "boot_allocator.h"
#include "buddy.h"
#include "panic.h"
#include "printk.h"
#include "utils.h"
#include <libft.h>

/*
 * TODO :
 *  Alloc Pages :
 *      Split Block
 *
 *  Free Pages :
 *      Coalescence Block
 *      Who is my buddy
 *
 *  Utils :
 *      Remove from list
 *      Add to list
 */

buddy_allocator_t buddy[ZONE_TYPES];

static inline uintptr_t reg_start(region_t reg) { return reg.start; }

static inline uintptr_t reg_end(region_t reg) { return reg.end; }

struct list_head *page_to_list_node(page_t *page)
{
	uintptr_t phys_addr = page_to_phys(page);
	return (struct list_head *)phys_addr;
}

page_t *list_node_to_page(struct list_head *node)
{
	uintptr_t phys_addr = (uintptr_t)node;
	return page_addr_to_page(phys_addr);
}

void buddy_list_add_head(struct list_head *new_node, struct list_head *head)
{
	new_node->next   = head->next;
	new_node->prev   = head;
	head->next->prev = new_node;
	head->next       = new_node;
}

void buddy_print(void)
{
	printk("Buddy Free Blocks: \n---\n");
	bool     has_blocks            = false;
	uint32_t total_pages_in_blocks = 0;

	for (int order = 0; order <= MAX_ORDER; order++) {
		uint32_t nr_free = buddy[NORMAL_ZONE].areas[order].nr_free;
		if (nr_free > 0) {
			uint32_t size_kb        = (1 << order) * 4;
			uint32_t pages_in_order = nr_free * (1 << order);
			total_pages_in_blocks += pages_in_order;
			printk("[%uKB:%u]\n", size_kb, nr_free);
			has_blocks = true;
		}
	}

	if (!has_blocks) {
		printk("(empty)");
	}
	printk("\n");
	printk("Total RAM : %u | Total pages : %u\n", total_RAM, total_pages);
	printk("Total pages in buddy blocks: %u\n", total_pages_in_blocks);
	printk("Free Pages : %u | Unusable pages : %u\n----\n", page_get_updated_free_count(),
	       page_get_updated_reserved_count());
	uint32_t reserved = 0, free = 0, allocated = 0, buddy = 0;
	for (uint32_t i = 0; i < total_pages; i++) {
		uint32_t flags = page_descriptors[i].flags;
		if (flags & PAGE_RESERVED)
			reserved++;
		if (flags & PAGE_ALLOCATED)
			allocated++;
		if (flags & PAGE_BUDDY)
			buddy++;
		if (!(flags & PAGE_RESERVED) && !(flags & PAGE_ALLOCATED))
			free++;
	}

	printk("Real Count : Reserved: %u, Allocated: %u, Buddy: %u, Free: %u\n", reserved, allocated,
	       buddy, free);
	printk("Total counted: %u (should be %u)\n----\n", reserved + allocated + buddy + free,
	       total_pages);
}

void buddy_init(void)
{
	boot_allocator_freeze();

	for (int order = 0; order <= MAX_ORDER; order++) {
		struct list_head *head = &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
		head->next             = head;
		head->prev             = head;
		buddy[NORMAL_ZONE].areas[order].nr_free = 0;
	}

	for (uint32_t i = 0; i < total_pages; i++) {
		if (PAGE_IS_UNUSABLE(&page_descriptors[i]))
			continue;
		page_t *first_page = &page_descriptors[i];
		while (PAGE_IS_FREE(&page_descriptors[i]) && i < total_pages)
			i++;
		if (PAGE_IS_UNUSABLE(&page_descriptors[i - 1]))
			break;
		page_t  *last_page    = &page_descriptors[i];
		uint32_t usable_size  = last_page - first_page;
		page_t  *current_page = first_page;
		for (int order = MAX_ORDER; order >= 0; order--) {
			uint32_t pages_per_block = (1 << order);

			while (usable_size >= pages_per_block) {
				struct list_head *new_node = page_to_list_node(current_page);
				buddy_list_add_head(new_node,
				                    &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE]);
				buddy[NORMAL_ZONE].areas[order].nr_free++;
				current_page += pages_per_block;
				usable_size -= pages_per_block;
			}
		}
	}
}