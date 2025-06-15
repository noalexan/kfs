#include "internal/buddy.h"
#include "internal/boot_allocator.h"
#include "panic.h"
#include "utils.h"
#include "vga.h"
#include <libft.h>

/////////////////////////////////////////////////////////////////////////////////
// Internal APIs

static buddy_allocator_t buddy[MAX_ZONE];

static inline struct list_head *ptr_to_list(void *ptr)
{
	return page_to_list_node(page_addr_to_page((uintptr_t)ptr));
}

static inline bool order_is_valid(int order) { return order >= 0 && order <= MAX_ORDER; }

static struct list_head *page_to_list_node(page_t *page)
{
	uintptr_t phys_addr = page_to_phys(page);
	return (struct list_head *)phys_addr;
}

static inline page_t *list_node_to_page(struct list_head *node)
{
	return page_addr_to_page((uintptr_t)node);
}

static void buddy_list_add_head(struct list_head *new_node, struct list_head *head)
{
	struct list_head *old_next = head->next;

	new_node->next = old_next;
	new_node->prev = head;
	old_next->prev = new_node;
	head->next     = new_node;
}

static inline struct list_head *order_to_free_list(size_t order)
{
	return &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
}

static inline size_t order_to_nrFree(size_t order)
{
	return buddy[NORMAL_ZONE].areas[order].nr_free;
}

static size_t size_to_order(size_t size)
{
	size_t total_pages = DIV_ROUND_UP(size, PAGE_SIZE);
	for (int order = 0; order <= MAX_ORDER; order++) {
		if (total_pages <= (size_t)PAGE_BY_ORDER(order))
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

static uintptr_t *pop_first_block(size_t order)
{
	struct list_head *head        = order_to_free_list(order);
	struct list_head *first_block = head->next;
	if (first_block == head)
		return NULL;

	buddy[NORMAL_ZONE].areas[order].nr_free--;
	return (uintptr_t *)pop_node(first_block);
}

static void set_block_metadata(uintptr_t *ret, size_t order, int type)
{
	page_t *first_page       = page_addr_to_page((uintptr_t)ret);
	first_page->private_data = type == FREE ? PAGE_MAGIC : (uintptr_t)order;
	type == FREE ? PAGE_SET_FREE(first_page) : PAGE_SET_ALLOCATED(first_page);
}

static uintptr_t *alloc_block_with_order(size_t order)
{
	uintptr_t *ret = pop_first_block(order);
	if (ret == NULL && order_to_nrFree(order) > 0)
		kpanic("Error : %s: free count mismatch (order %u, nrFree=%u)", __func__, order,
		       order_to_nrFree(order));
	if (ret)
		set_block_metadata(ret, order, ALLOCATED);
	return ret;
}

static uintptr_t *split_block_to_order(size_t order_needed, size_t cur_order, uintptr_t *ret)
{
	if (!order_is_valid(order_needed) || !order_is_valid(cur_order))
		kpanic("Error: %s: Invalid Order\n", __func__);
	if (cur_order == order_needed) {
		set_block_metadata(ret, order_needed, ALLOCATED);
		return ret;
	}
	page_t *first_page = page_addr_to_page((uintptr_t)ret);
	page_t *buddy_page = first_page + PAGE_BY_ORDER((cur_order - 1));

	buddy_list_add_head(page_to_list_node(buddy_page), order_to_free_list(cur_order - 1));
	buddy[NORMAL_ZONE].areas[cur_order - 1].nr_free++;
	cur_order--;
	return split_block_to_order(order_needed, cur_order, ret);
}

static uintptr_t get_buddy_base(uintptr_t addr)
{
	size_t    free_count = boot_allocator_get_region_count(FREE_MEMORY);
	region_t *free_reg   = boot_allocator_get_region(FREE_MEMORY);

	for (size_t i = 0; i < free_count; i++) {
		if (addr >= free_reg[i].start && addr < free_reg[i].end)
			return free_reg[i].start;
	}
	return 0;
}

static struct list_head *get_buddy_node(void *block, size_t order)
{
	struct list_head *head = order_to_free_list(order);
	struct list_head *cur  = head->next;
	uintptr_t         buddy_addr =
	    WHO_IS_MY_BUDDY((uintptr_t)block, order, get_buddy_base((uintptr_t)block));

	while (cur != head) {
		if ((uintptr_t)cur == buddy_addr) {
			return cur;
		}
		cur = cur->next;
	}
	print_buddy_free_list(order);

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////
// Externals APIs

void buddy_print(void)
{
	vga_printf("Buddy Free Blocks: \n---\n");
	bool   has_blocks            = false;
	size_t total_pages_in_blocks = 0;

	for (int order = 0; order <= MAX_ORDER; order++) {
		size_t nr_free = buddy[NORMAL_ZONE].areas[order].nr_free;
		if (nr_free > 0) {
			size_t pages_in_order = nr_free * PAGE_BY_ORDER(order);
			total_pages_in_blocks += pages_in_order;
			vga_printf("[%s:%u] ", debug_buddy_order_to_string(order), nr_free);
			has_blocks = true;
		}
	}
	vga_printf("\n");
	vga_printf("Total RAM : %u | Total pages : %u\n", total_RAM, total_pages);
	vga_printf("Total pages in buddy blocks: %u\n", total_pages_in_blocks);
	vga_printf("Free Pages : %u | Unusable pages : %u\n----\n", page_get_updated_free_count(),
	           page_get_updated_reserved_count());
	if (!has_blocks) {
		vga_printf("(empty)");
	}
}

size_t buddy_get_var_size(void *var)
{
	page_t *page = page_addr_to_page((uintptr_t)var);
	if (PAGE_DATA_IS_MAGIC(page))
		return 0;
	return PAGE_BY_ORDER(page->private_data) * PAGE_SIZE;
}

uintptr_t *buddy_alloc_pages(size_t size)
{
	size_t order_needed = size_to_order(size);
	size_t cur_order    = order_needed;
	while (cur_order != BAD_ORDER && !order_to_nrFree(cur_order))
		cur_order++;
	if (cur_order == BAD_ORDER)
		return NULL;
	if (cur_order != order_needed) {
		uintptr_t *ret = pop_first_block(cur_order);
		if (ret == NULL)
			return NULL;
		return split_block_to_order(order_needed, cur_order, ret);
	}
	return alloc_block_with_order(order_needed);
}

void buddy_free_block(void *ptr)
{
	page_t *page = page_addr_to_page((uintptr_t)ptr);

	size_t block_order = (size_t)page->private_data;
	if (!order_is_valid(block_order)) {
		kpanic("Error: buddy_free_block: incorrect block order: %x\n", block_order);
	}

	page->private_data = PAGE_MAGIC;
	PAGE_SET_FREE(page);

	while (block_order < MAX_ORDER) {

		struct list_head *buddy_node = get_buddy_node(ptr, block_order);
		if (!buddy_node) {
			break;
		}

		buddy_node = pop_node(buddy_node);

		page_t *buddy_page = page_addr_to_page((uintptr_t)buddy_node);

		buddy_page->private_data = PAGE_MAGIC;
		PAGE_SET_FREE(buddy_page);

		buddy[NORMAL_ZONE].areas[block_order].nr_free--;
		ptr = (void *)MIN((uintptr_t)ptr, (uintptr_t)buddy_node);

		block_order++;
	}

	buddy_list_add_head(ptr_to_list(ptr), order_to_free_list(block_order));
	buddy[NORMAL_ZONE].areas[block_order].nr_free++;
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

	size_t    free_count = boot_allocator_get_region_count(FREE_MEMORY);
	region_t *free_reg   = boot_allocator_get_region(FREE_MEMORY);

	for (size_t i = 0; i < free_count; i++) {
		page_t *region_start_page = page_addr_to_usable(free_reg[i].start, NEXT);
		page_t *region_end_page   = page_addr_to_usable(free_reg[i].end, PREV);

		size_t  usable_page  = region_end_page - region_start_page + 1;
		page_t *current_page = region_start_page;

		for (int order = MAX_ORDER; order >= 0; order--) {
			size_t pages_per_block = PAGE_BY_ORDER(order);
			while (usable_page >= pages_per_block) {
				struct list_head *new_node = (struct list_head *)page_to_phys(current_page);
				ft_memset(new_node, 0, sizeof(struct list_head));

				buddy_list_add_head(new_node,
				                    &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE]);
				buddy[NORMAL_ZONE].areas[order].nr_free++;
				current_page += pages_per_block;
				usable_page -= pages_per_block;
			}
		}
		if (usable_page != 0)
			kpanic("%d page lost in the hood\n", usable_page);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// DEBUG
/////////////////////////////////////////////////////////////////////////////////

static const char *debug_buddy_order_to_string(int order)
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

static void print_buddy_free_list(size_t order)
{
	struct list_head *head     = &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];
	struct list_head *cur      = head->next;
	int               node_num = 0;

	debug_buddy_corrupted_list(order);

	vga_printf("=== Free list [order %s] ===\n", debug_buddy_order_to_string(order));
	vga_printf("Head : %p\n Size : %d\n", head, order_to_nrFree(order));
	while (cur != head) {
		vga_printf("  Node %d : %p | next = %p | prev = %p\n", node_num, cur, cur->next, cur->prev);
		cur = cur->next;
		node_num++;
	}
}

static void debug_buddy_panic(const char *func)
{
	vga_printf("  Panic system here %s()\n", func);
	vga_printf("=========================================\n");
	halt();
}

static void debug_buddy_print_node_info(struct list_head *node)
{
	if (!node) {
		vga_printf("    [Node State] NULL pointer\n");
		return;
	}
	vga_printf("    [Node %p State] | next: %p | prev: %p\n", node, node->next, node->prev);
	if (node->next)
		vga_printf("        -> node->next->prev: %p (should be %p)\n", node->next->prev, node);
}

static void debug_buddy_corrupted_list(size_t order)
{
	struct list_head *cur;
	size_t            count = 0;
	size_t            size  = order_to_nrFree(order);
	size_t            limit = (size == 0 ? 2000 : size * 4);
	struct list_head *head  = &buddy[NORMAL_ZONE].areas[order].free_list[MIGRATE_MOVABLE];

	for (cur = head->next; cur != head; cur = cur ? cur->next : NULL) {
		count++;
		if (!cur) {
			vga_printf("\n\n--- KERNEL PANIC: LIST CORRUPTION ---\n");
			vga_printf("  Buddy free_list for order %u is corrupted!\n\n", order);
			vga_printf("  Reason: NULL pointer encountered during traversal (node #%zu)\n", count);
			debug_buddy_print_node_info(cur);
			debug_buddy_panic(__func__);
		}
		if (count > limit) {
			vga_printf("\n\n--- KERNEL PANIC: LIST CORRUPTION ---\n");
			vga_printf("  Buddy free_list for order %u is corrupted!\n\n", order);
			vga_printf("  Reason: Infinite loop or corruption detected (node limit exceeded)\n");
			debug_buddy_print_node_info(cur);
			debug_buddy_panic(__func__);
		}
	}
	// vga_printf("[OK] Buddy free_list for order %s is healthy\n",
	// debug_buddy_order_to_string(order), count);
}

static void debug_buddy_check_lost_pages(void)
{
	size_t    lost        = 0;
	size_t    total_buddy = 0;
	size_t    free_count  = boot_allocator_get_region_count(FREE_MEMORY);
	region_t *free_reg    = boot_allocator_get_region(FREE_MEMORY);

	for (uint32_t i = 0; i < total_pages; i++) {
		if (PAGE_IS_FREE(&page_descriptors[i])) {
			uintptr_t addr     = i * PAGE_SIZE;
			bool      in_buddy = false;
			for (size_t j = 0; j < free_count; j++) {
				if (addr >= free_reg[j].start && addr < free_reg[j].end) {
					in_buddy = true;
					break;
				}
			}
			if (!in_buddy) {
				vga_printf("Lost page: 0x%x\n", addr);
				lost++;
			} else {
				total_buddy++;
			}
		}
	}
	// vga_printf("Total free pages (descriptor): %u\n", page_get_updated_free_count());
	// vga_printf("Total free pages (in buddy regions): %u\n", total_buddy);
	if (lost > 0) {
		vga_printf("\n\n--- KERNEL PANIC: PAGES Missing ---\n");
		vga_printf("Total lost pages: %u\n", lost);
		debug_buddy_panic(__func__);
	}
}

static void debug_buddy_alloc_still_free(size_t order, uintptr_t *phys)
{
	bool              found = false;
	struct list_head *head  = order_to_free_list(order);
	struct list_head *cur   = head->next;

	while (cur != head) {
		if ((uintptr_t *)cur == phys) {
			found = true;
			break;
		}
		cur = cur->next;
	}
	if (found) {
		vga_printf("Error: %s : (%p) is STILL present in %s free list!\n", __func__, (void *)phys,
		           debug_buddy_order_to_string(order));
		debug_buddy_panic(__func__);
	}
	// else {
	// vga_printf("OK: %s: (%p) is not present in %s free list!\n",
	//    __func__, (void *)phys, debug_buddy_order_to_string(order));
	// }
}

static void buddy_drain_lower_orders(void)
{
	for (int order = 0; order < MAX_ORDER; order++) {
		size_t free_blocks = order_to_nrFree(order);
		while (free_blocks-- > 0) {
			uintptr_t *blk = alloc_block_with_order(order);
			if (blk == NULL) {
				vga_printf("Error: Unexpected NULL while draining order %d\n", order);
				debug_buddy_panic(__func__);
			}
		}
		uintptr_t *blk = alloc_block_with_order(order);
		if (blk != NULL) {
			vga_printf("Error: After draining, order %d still has a free block (%p)\n", order,
			           (void *)blk);
			debug_buddy_panic(__func__);
		}
		buddy[NORMAL_ZONE].areas[order].nr_free = 0;
	}
}

static void debug_buddy_simple_alloc(void)
{
	for (int i = MAX_ORDER; i >= 0; i--) {
		if (order_to_nrFree(i) > 0) {
			uintptr_t *ret = alloc_block_with_order(i);
			debug_buddy_alloc_still_free(i, ret);
		}
	}
}

static void debug_buddy_check_all_list_corrumption(void)
{
	for (size_t order = 0; order <= MAX_ORDER; order++)
		debug_buddy_corrupted_list(order);
}

static void debug_buddy_split_block(void)
{
	buddy_drain_lower_orders();

	for (int order = MAX_ORDER - 1; order >= 0; order--) {
		uintptr_t *block = buddy_alloc_pages(ORDER_TO_BYTES(order));
		debug_buddy_alloc_still_free(order, block);
		for (int i = order; i < MAX_ORDER; i++) {
			if (order_to_nrFree(i) != 1) {
				vga_printf("Error: order %d should have exactly 1 free block, found %u\n", i,
				           order_to_nrFree(i));
				print_buddy_free_list(i);
				debug_buddy_panic(__func__);
			}
		}
		debug_buddy_check_all_list_corrumption();
		debug_buddy_simple_alloc();
		buddy_drain_lower_orders();
	}
}

static void debug_buddy_free_block(void)
{
	buddy_drain_lower_orders();

	for (size_t order = 0; order <= MAX_ORDER; order++) {
		uintptr_t *block = buddy_alloc_pages(ORDER_TO_BYTES(order));
		debug_buddy_alloc_still_free(order, block);

		buddy_free_block(block);

		for (size_t i = order; i < MAX_ORDER; i++) {
			if (order_to_nrFree(i) != 0) {
				vga_printf("Error: order %u should have exactly 0 free block, found %u\n", i,
				           order_to_nrFree(i));
				print_buddy_free_list(i);
				debug_buddy_panic(__func__);
			}
		}
	}
}

void debug_buddy(void)
{

	// Missing Pages
	debug_buddy_check_lost_pages();
	vga_printf("[OK] No pages missing\n");

	// List Corrumption
	debug_buddy_check_all_list_corrumption();
	vga_printf("[OK] Buddy free_list for order %s is healthy\n");

	// Simple alloc
	debug_buddy_simple_alloc();
	debug_buddy_check_all_list_corrumption();
	vga_printf("[OK] First test for simple allocation\n");

	// Split allocation
	debug_buddy_split_block();
	vga_printf("[OK] Split Blocks\n");

	// Free Blocks
	debug_buddy_free_block();
	vga_printf("[OK] Free Blocks\n");

	// Free Invalid Pointer
	// buddy_drain_lower_orders();
	// buddy_free_block(0x01000);

	// Alloc Invalid Size
	buddy_drain_lower_orders();
}
