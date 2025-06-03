#include "buddy.h"
#include "boot_allocator.h"
#include "buddy.h"
#include "panic.h"
#include "printk.h"
#include "utils.h"
#include <libft.h>

/*
 * TODO :
 *  Free Pages :
 *      Coalescence Block
 *      Who is my buddy
 */

buddy_allocator_t buddy[ZONE_TYPES];

static struct list_head *page_to_list_node(page_t *page)
{
	uintptr_t phys_addr = page_to_phys(page);
	return (struct list_head *)phys_addr;
}

static page_t *list_node_to_page(struct list_head *node)
{
	uintptr_t phys_addr = (uintptr_t)node;
	return page_addr_to_page(phys_addr);
}

static void buddy_list_add_head(struct list_head *new_node, struct list_head *head)
{
	new_node->next   = head->next;
	new_node->prev   = head;
	head->next->prev = new_node;
	head->next       = new_node;
}

static inline struct list_head *order_to_free_list(uint32_t order)
{
	return &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
}

static inline uint32_t get_nrFree_by_order(uint32_t order)
{
	return buddy[NORMAL_ZONE].areas[order].nr_free;
}

static uint32_t size_to_order(uint32_t size)
{
	uint32_t total_pages = DIV_ROUND_UP(size, PAGE_SIZE);
	for (int order = 0; order <= MAX_ORDER; order++) {
		if (total_pages <= (uint32_t)PAGE_BY_ORDER(order))
			return order;
	}
	return BAD_ORDER;
}

static uintptr_t *detach_and_return_block(uint32_t order)
{
	struct list_head *head        = order_to_free_list(order);
	struct list_head *first_block = head->next;
	if (head->next == head)
		return NULL;
	head->next              = first_block->next;
	first_block->next->prev = head;

	first_block->next = NULL;
	first_block->prev = NULL;

	buddy[NORMAL_ZONE].areas[order].nr_free--;

	return (uintptr_t *)first_block;
}

static uintptr_t *set_metadata_and_return_block(uint32_t order)
{
	uintptr_t *ret           = detach_and_return_block(order);
	page_t    *first_page    = page_addr_to_page((uintptr_t)ret);
	first_page->private_data = (uintptr_t)order;
	PAGE_SET_ALLOCATED(first_page);
	return ret;
}

static uintptr_t *split_blocks(uint32_t order_needed, uint32_t cur_order)
{
	if (cur_order == order_needed)
		return set_metadata_and_return_block(order_needed);
	uintptr_t *first_block = detach_and_return_block(cur_order--);
	if (first_block == NULL)
		return NULL;
	page_t *first_page = page_addr_to_page(*first_block);
	page_t *next_page  = first_page + PAGE_BY_ORDER(cur_order);
	buddy_list_add_head(page_to_list_node(next_page), order_to_free_list(cur_order));
	buddy_list_add_head(page_to_list_node(first_page), order_to_free_list(cur_order));
	buddy[NORMAL_ZONE].areas[cur_order].nr_free += 2;
	return split_blocks(order_needed, cur_order);
}

/////////////////////////////////////////////////////////////////////////////////
// Externals APIs

uint32_t buddy_get_var_size(void *var)
{
	page_t *page = page_addr_to_page((uintptr_t)var);
	if (page->private_data == PAGE_MAGIC)
		return 0;
	return PAGE_BY_ORDER(page->private_data) * PAGE_SIZE;
}

uintptr_t *buddy_alloc_pages(uint32_t size)
{
	uint32_t order_needed = size_to_order(size);
	uint32_t cur_order    = order_needed;
	while (cur_order != BAD_ORDER && !get_nrFree_by_order(cur_order))
		cur_order++;
	if (cur_order == BAD_ORDER)
		return NULL;
	if (cur_order != order_needed)
		return split_blocks(order_needed, cur_order);
	return set_metadata_and_return_block(order_needed);
}

void buddy_print(void)
{
	printk("Buddy Free Blocks: \n---\n");
	bool     has_blocks            = false;
	uint32_t total_pages_in_blocks = 0;

	for (int order = 0; order <= MAX_ORDER; order++) {
		uint32_t nr_free = buddy[NORMAL_ZONE].areas[order].nr_free;
		if (nr_free > 0) {
			uint32_t size_kb        = PAGE_BY_ORDER(order) * 4;
			uint32_t pages_in_order = nr_free * PAGE_BY_ORDER(order);
			total_pages_in_blocks += pages_in_order;
			printk("[%uKB:%u] ", size_kb, nr_free);
			has_blocks = true;
		}
	}
	printk("\n");
	printk("Total RAM : %u | Total pages : %u\n", total_RAM, total_pages);
	printk("Total pages in buddy blocks: %u\n", total_pages_in_blocks);
	printk("Free Pages : %u | Unusable pages : %u\n----\n", page_get_updated_free_count(),
	       page_get_updated_reserved_count());
	if (!has_blocks) {
		printk("(empty)");
	}
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
			uint32_t pages_per_block = PAGE_BY_ORDER(order);

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
