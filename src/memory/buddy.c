#include "buddy.h"
#include "boot_allocator.h"
#include "buddy.h"
#include "panic.h"
#include "printk.h"
#include "utils.h"
#include <libft.h>

/////////////////////////////////////////////////////////////////////////////////
// DEBUG

const char *debug_buddy_order_to_string(int order)
{
	switch (order) {
	case 0:
		return "4KiB";
	case 1:
		return "8KiB";
	case 2:
		return "16KiB";
	case 3:
		return "32KiB";
	case 4:
		return "64KiB";
	case 5:
		return "128KiB";
	case 6:
		return "256KiB";
	case 7:
		return "512KiB";
	case 8:
		return "1MiB";
	case 9:
		return "2MiB";
	case 10:
		return "4MiB";
	default:
		return "Unknown order";
	}
}

void print_buddy_free_list(uint32_t order)
{
	struct list_head *head  = &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
	struct list_head *cur   = head->next;
	int               count = 0;
	printk("=== Free list [order %u] ===\n", order);
	printk("Head @ %p\n", head);

	while (cur != head) {
		// Affiche l'adresse du nœud courant et ses pointeurs
		printk("  Node #%d @ %p | next = %p | prev = %p\n", count, cur, cur->next, cur->prev);

		// Optionnel : remonter à la page associée si tu as une fonction pour ça
		// page_t *page = phys_to_page((uintptr_t)cur);
		// printk("    -> page index = %u\n", page_to_index(page));

		cur = cur->next;
		count++;

		// Sécurité anti-boucle infinie
		if (count > 800) {
			printk("  !! Boucle infinie ou corruption détectée (trop de blocs)\n");
			break;
		}
	}
	printk("Total blocks in free_list[%u] : %d\n", order, count);
	printk("=============================\n");
}

void debug_buddy_free_list(uint32_t order)
{
	printk("---\nIN %s | curent free block %d!\n", __func__, order_to_nrFree(order));

	struct list_head *head  = order_to_free_list(order);
	struct list_head *cur   = head->next;
	int               count = 0;
	int               max   = order_to_nrFree(order) * 4;
	while (cur != head && count < max) {
		if (order == 6) {
			printk("	Why: stuck here count(%d) max(%d) head(%p) cur(%p)!\n", count, max, head,
			       cur);
		}
		cur = cur->next;
		count++;
	}
	if (cur != head) {
		printk("-	!! Infinite loop or corruption detected in free_list[%d] (count=%d)\n", order,
		       count);
		halt();
	} else {
		printk("-	OK: free_list [order %d], nodes traversed: %d\n", order, count);
	}
	printk("OUT %s!\n---\n", __func__);
}

void debug_buddy_alloc_still_free(uint32_t order, uintptr_t *phys)
{
	printk("---\nIN %s!\n", __func__);
	if (order_to_nrFree(order) == 0) {
		printk("	MAY: no block, nothing to be done here %s!\n", __func__);
		goto out;
	}
	struct list_head *head  = order_to_free_list(order);
	struct list_head *cur   = head->next;
	int               found = 0;
	int               count = 0;
	int               max   = order_to_nrFree(order) * 4;
	while (cur != head && count < max) {
		if (order == 6) {
			printk("	Why: stuck here count(%d) max(%d) head(%p) cur(%p)!\n", count, max, head,
			       cur);
		}
		if ((uintptr_t)cur == phys) {
			found = 1;
			break;
		}
		cur = cur->next;
		count++;
	}
	if (found) {
		printk("-	!! ERROR: %s: (%p) is STILL present in %s!\n", __func__, (void *)phys,
		       debug_buddy_order_to_string(order));
		halt();
	} else if (count == max) {
		printk("-	!! Infinite loop or corruption detected in free_list[%d] (count=%d)\n", order,
		       count);
		halt();
	} else
		printk("-	OK: %s: (%p) is not present in his list!\n", __func__, (void *)phys);
out:
	printk("OUT %s!\n---\n", __func__);
}

void debug_buddy_simple_alloc(int i)
{
	printk("=====================================================================\n");
	printk("\n\n=== TEST BUDDY ORDER %d (%s) with %d free_blocks ===\n\n", i,
	       debug_buddy_order_to_string(i), order_to_nrFree(i));
	uintptr_t *blk1 = pop_block_with_order(i);
	debug_buddy_free_list(i);
	debug_buddy_alloc_still_free(i, blk1);
	uintptr_t *blk2 = pop_block_with_order(i);
	if (blk1 != NULL && blk2 != NULL) {
		if (blk1 == blk2) {
			printk("-	Error: Pop Smoke is die with %x at %d order\n", blk1, i);
			halt();
		} else {
			printk("-	OK: Who can say Pop without Smoke %x %x\n", blk1, blk2);
		}

		debug_buddy_alloc_still_free(i, blk2);
		debug_buddy_free_list(i);
	} else if (blk1 == NULL) {
		printk("-	!! Allocation failed for order %d\n", i);
		halt();
		return;
	}
}

void debug_buddy(void)
{
	// Simple alloc
	for (int i = MAX_ORDER; i >= 0; i--) {
		// printk("=====================================================================\n");
		if (order_to_nrFree(i) > 0) {
			debug_buddy_simple_alloc(i);
		} else {
			printk("\n\n=====================================================================\n\n");
			printk("Order %d need split for bigger block\n\n", i);
		}
	}
	// Alloc With Split
}
uint32_t kord = 12;

/////////////////////////////////////////////////////////////////////////////////
// Internal APIs

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

	new_node->next = NULL;
	new_node->prev = NULL;
	if (kord < 7) {
		printk("0 newn(%p) newp(%p) | headn(%p) headp(%p)\n", new_node->next, new_node->prev,
		       head->next, head->prev);
	}
	new_node->next = head->next;
	new_node->prev = head;
	if (kord < 7) {
		printk("1 newn(%p) newp(%p) | headn(%p) headp(%p)\n", new_node->next, new_node->prev,
		       head->next, head->prev);
	}
	head->next->prev = new_node;
	head->next       = new_node;
	if (kord < 7) {
		printk("2 newn(%p) newp(%p) | headn(%p) headp(%p)\n", new_node->next, new_node->prev,
		       head->next, head->prev);
	}
}

static inline struct list_head *order_to_free_list(uint32_t order)
{
	return &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
}

static inline uint32_t order_to_nrFree(uint32_t order)
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

static struct list_head *pop_node(struct list_head *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next       = NULL;
	node->prev       = NULL;
	return node;
}

static uintptr_t *pop_first_block(uint32_t order)
{
	struct list_head *head        = order_to_free_list(order);
	struct list_head *first_block = head->next;
	if (first_block == head)
		return NULL;

	buddy[NORMAL_ZONE].areas[order].nr_free--;
	return (uintptr_t *)pop_node(first_block);
}

static void set_block_metadata(uintptr_t *ret, uint32_t order, int type)
{
	page_t *first_page       = page_addr_to_page((uintptr_t)ret);
	first_page->private_data = type == FREE ? PAGE_MAGIC : (uintptr_t)order;
	type == FREE ? PAGE_SET_FREE(first_page) : PAGE_SET_ALLOCATED(first_page);
}

static inline uintptr_t *pop_block_with_order(uint32_t order)
{
	if (!buddy[NORMAL_ZONE].areas[order].nr_free)
		return NULL;
	uintptr_t *ret = pop_first_block(order);
	set_block_metadata(ret, order, ALLOCATED);
	return ret;
}

static uintptr_t *split_blocks(uint32_t order_needed, uint32_t cur_order, uintptr_t *ret)
{
	if (cur_order == order_needed) {
		set_block_metadata(ret, order_needed, ALLOCATED);
		return ret;
	}
	page_t *first_page = page_addr_to_page((uintptr_t)ret);
	page_t *next_page  = first_page + PAGE_BY_ORDER(cur_order);
	buddy_list_add_head(page_to_list_node(next_page), order_to_free_list(cur_order));
	printk("block A %x, block b %x", first_page, next_page);
	buddy[NORMAL_ZONE].areas[cur_order].nr_free++;
	cur_order--;
	return split_blocks(order_needed, cur_order, ret);
}

uintptr_t *buddy_alloc_pages(uint32_t size)
{
	uint32_t order_needed = size_to_order(size);
	uint32_t cur_order    = order_needed;
	while (cur_order != BAD_ORDER && !order_to_nrFree(cur_order))
		cur_order++;
	if (cur_order == BAD_ORDER)
		return NULL;
	if (cur_order != order_needed) {
		uintptr_t *ret = pop_first_block(cur_order);
		if (ret == NULL)
			return NULL;
		printk("BLOCK NEED TO BE SPLITTED\n");
		return split_blocks(order_needed, cur_order, ret);
	}
	printk("BLOCK DONT NEED TO BE SPLITED\n");
	return pop_block_with_order(order_needed);
}

/////////////////////////////////////////////////////////////////////////////////
// Externals APIs

void buddy_free_block(void *ptr)
{
	// Check if the ptr page is correct is correct
	page_t *page = page_addr_to_page((uintptr_t)ptr);
	if (PAGE_IS_FREE(page) || PAGE_DATA_IS_MAGIC(page))
		kpanic("error: buddy_free_block: trying to free a non-allocated block\n");

	uint32_t block_order = (uint32_t)page->private_data;
	if (block_order > MAX_ORDER) {
		kpanic("error: buddy_free_block: incorrect block order: %x\n", block_order);
	}
	page->private_data = PAGE_MAGIC;
	PAGE_SET_FREE(page);
	debug_buddy_free_list(block_order);
	while (block_order <= MAX_ORDER) {
		uintptr_t         buddy_addr = WHO_IS_MY_BUDDY((uintptr_t)ptr, block_order);
		struct list_head *first_node = order_to_free_list(block_order);
		struct list_head *cur        = first_node->next;
		while (cur != first_node) {
			if ((uintptr_t)cur == buddy_addr)
				break;
			cur = cur->next;
		}
		if (cur == first_node)
			break;
		cur = pop_node(cur);
		buddy[NORMAL_ZONE].areas[block_order].nr_free--;
		ptr = (void *)MIN((uintptr_t)ptr, (uintptr_t)cur);
		if (block_order == MAX_ORDER)
			break;
		block_order++;
	}
	buddy_list_add_head(page_to_list_node(page_addr_to_page((uintptr_t)ptr)),
	                    order_to_free_list(block_order));
	buddy[NORMAL_ZONE].areas[block_order].nr_free++;
}

uint32_t buddy_get_var_size(void *var)
{
	page_t *page = page_addr_to_page((uintptr_t)var);
	if (PAGE_DATA_IS_MAGIC(page))
		return 0;
	return PAGE_BY_ORDER(page->private_data) * PAGE_SIZE;
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
		while (i < total_pages && PAGE_IS_FREE(&page_descriptors[i])) {
			i++;
		}
		page_t  *last_page    = &page_descriptors[i];
		uint32_t usable_size  = last_page - first_page;
		page_t  *current_page = first_page;
		printk("---------------------\nbuddy init : first_page(%p), last_page(%p) , "
		       "usable_size(%d)\n\n",
		       usable_size);
		for (int order = MAX_ORDER; order >= 0; order--) {
			uint32_t pages_per_block = PAGE_BY_ORDER(order);
			while (usable_size >= pages_per_block) {
				struct list_head *new_node = page_to_list_node(current_page);
				if (order < 7) {
					kord                 = order;
					uintptr_t start_addr = page_to_phys(current_page);
					uintptr_t end_addr   = start_addr + pages_per_block * PAGE_SIZE;
					printk("%s : s(%p) e(%p)\n", debug_buddy_order_to_string(order),
					       (void *)start_addr, (void *)end_addr);
				}
				buddy_list_add_head(new_node,
				                    &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE]);
				buddy[NORMAL_ZONE].areas[order].nr_free++;
				current_page += pages_per_block;
				usable_size -= pages_per_block;
			}
		}
	}
	kord = 12;
}
