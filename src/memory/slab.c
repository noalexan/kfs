#include <drivers/vga.h>
#include <kernel/panic.h>
#include <memory/buddy.h>
#include <memory/kmalloc.h>
#include <memory/memory.h>
#include <memory/page.h>
#include <memory/slab.h>
#include <types.h>
#include <utils.h>

// ============================================================================
// IMCLUDES
// ============================================================================

// ============================================================================
// DEFINE AND MACRO
// ============================================================================

// Defines

// Macros
#define SLAB_INTRUSIVE_THRESHOLD 512
#define INIT_SENTINEL(name, ptr)                                                                   \
	do {                                                                                           \
		struct list_head *name = ptr;                                                              \
		name->next             = name;                                                             \
		name->prev             = name;                                                             \
	} while (0)
#define SLAB_IS_EXTERNAL(size) ((size) > SLAB_INTRUSIVE_THRESHOLD)

// ============================================================================
// STRUCT
// ============================================================================

// Enums

typedef enum {
	CACHE_8B = 0,
	CACHE_16B,

	CACHE_32B,
	CACHE_64B,
	CACHE_128B,
	CACHE_256B,
	CACHE_512B,
	CACHE_1024B,
	CACHE_2048B,
	NUM_SLAB_CACHES,
} cache_size;

// Structures

// ============================================================================
// VARIABLES GLOBALES
// ============================================================================

static const size_t cache_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};

static slab_cache_t slab_caches[MAX_ZONE][NUM_SLAB_CACHES];

// ============================================================================
// INTERNAL APIs
// ============================================================================

static void list_add_head(struct list_head *new_node, struct list_head *head)
{
	struct list_head *old_next = head->next;

	new_node->next = old_next;
	new_node->prev = head;
	old_next->prev = new_node;
	head->next     = new_node;
}

static bool list_is_empty(struct list_head *lst)
{
	if (!lst)
		return true;
	else if (lst == lst->next)
		return true;
	return false;
}

static void pop_node(struct list_head *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next       = NULL;
	node->prev       = NULL;
}

static struct list_head *list_first_entry(struct list_head *lst)
{
	if (!list_is_empty(lst))
		return lst->next;
	return NULL;
}

static cache_size size_to_cache_size(size_t size)
{
	if (size <= 8)
		return CACHE_8B;

	size_t cache_idx  = CACHE_16B;
	size_t cache_size = 16;

	while (cache_size < size) {
		cache_size *= 2;
		cache_idx++;
	}

	if (cache_idx >= NUM_SLAB_CACHES)
		return NUM_SLAB_CACHES;

	return cache_idx;
}

static void slab_init_intrusive_freelist(slab_t *src, void *first_obj, size_t obj_size,
                                         size_t available_space)
{
	char  *current       = (char *)first_obj;
	size_t end_iteration = available_space / obj_size;

	for (size_t i = 0; i < end_iteration - 1; i++) {
		void *next        = (char *)(current + obj_size);
		*(void **)current = next;
		current           = next;
	}
	*(void **)current = NULL;
	src->freelist     = first_obj;
}

static slab_t *slab_create(slab_cache_t *cache, zone_type zone)
{
	void *new_page_phy = buddy_alloc_pages(PAGE_SIZE, zone);
	if (new_page_phy == NULL) {
		return NULL;
	}
	page_t *page = page_addr_to_page((uintptr_t)new_page_phy);
	PAGE_SET_STATE(page, PAGE_STATE_SLAB);

	slab_t *ret;
	void   *first_object;
	size_t  available_space;
	void   *new_page_virt = PHYS_TO_VIRT_LINEAR(new_page_phy);
	if (SLAB_IS_EXTERNAL(cache->object_size)) {
		/* Using kmalloc here is really smart,
		 * but we need to keep in mind that if this current kernel implementation supports multi-core (SMP) architecture,
		 * concurrency can create race conditions or at least deadlocks.
		 */
		ret = kmalloc(sizeof(slab_t), GFP_KERNEL);
		if (!ret) {
			buddy_free_block(new_page_phy);
			return NULL;
		}
		first_object    = new_page_virt;
		available_space = PAGE_SIZE;

	} else {
		ret             = (slab_t *)new_page_virt;
		first_object    = (void *)((uintptr_t)new_page_virt + sizeof(slab_t));
		available_space = PAGE_SIZE - sizeof(slab_t);
	}

	ret->inuse        = 0;
	ret->parent_cache = cache;
	slab_init_intrusive_freelist(ret, first_object, cache->object_size, available_space);
	page->private_data = (uintptr_t)ret;
	return ret;
}

// ============================================================================
// EXTERNAL APIs
// ============================================================================

void slab_free(void *ptr)
{
	page_t *page = page_addr_to_page((uintptr_t)VIRT_TO_PHYS_LINEAR(ptr));
	if (PAGE_GET_STATE(page) != PAGE_STATE_SLAB)
		kpanic("Error: %s: try to free memory not handled by slab.\n", __func__);
	slab_t       *slab  = (slab_t *)page->private_data;
	slab_cache_t *cache = slab->parent_cache;

	char *obj      = ptr;
	*(void **)obj  = slab->freelist;
	slab->freelist = obj;
	slab->inuse--;
	if (slab->inuse == cache->objects_per_slab - 1) {
		pop_node(&slab->list);
		list_add_head(&slab->list, &cache->slabs_partial);
	} else if (slab->inuse == 0) {
		// TODO : implement a memory shrinker logic to simplify logic and don't implement a
		// memory reclaimer
		pop_node(&slab->list);
		list_add_head(&slab->list, &cache->slabs_empty);
	}
}

void *slab_alloc(size_t size, zone_type zone)
{

	cache_size slab_size = size_to_cache_size(size);
	if (slab_size == NUM_SLAB_CACHES || zone > DMA_ZONE)
		return NULL;

	slab_cache_t *cache = &slab_caches[zone][slab_size];
	slab_t       *slab;

	if (!list_is_empty(&cache->slabs_partial))
		slab = container_of(list_first_entry(&cache->slabs_partial), slab_t, list);
	else if (!list_is_empty(&cache->slabs_empty)) {
		slab = container_of(list_first_entry(&cache->slabs_empty), slab_t, list);
		pop_node(&slab->list);
		list_add_head(&slab->list, &cache->slabs_partial);
	} else {
		slab = slab_create(cache, zone);
		// Out Of memory
		if (slab == NULL)
			return NULL;
		list_add_head(&slab->list, &cache->slabs_partial);
	}

	void *ret      = slab->freelist;
	slab->freelist = *(void **)ret;
	slab->inuse++;

	if (slab->freelist == NULL) {
		pop_node(&slab->list);
		list_add_head(&slab->list, &cache->slabs_full);
	}
	return ret;
}

void slab_init(void)
{
	for (size_t zone = 0; zone < MAX_ZONE; zone++) {
		for (int i = 0; i < NUM_SLAB_CACHES; i++) {
			INIT_SENTINEL(full, &slab_caches[zone][i].slabs_full);
			INIT_SENTINEL(partial, &slab_caches[zone][i].slabs_partial);
			INIT_SENTINEL(empty, &slab_caches[zone][i].slabs_empty);
			slab_caches[zone][i].object_size = cache_sizes[i];
			slab_caches[zone][i].objects_per_slab =
			    (SLAB_IS_EXTERNAL(cache_sizes[i]) ? PAGE_SIZE : PAGE_SIZE - sizeof(slab_t)) /
			    cache_sizes[i];
		}
	}
}

// ============================================================================
// DEBUG APIs
// ============================================================================

static size_t list_count_nodes(struct list_head *head)
{
	size_t            count = 0;
	struct list_head *iter;
	for (iter = head->next; iter != head; iter = iter->next) {
		count++;
	}
	return count;
}

static const char *debug_slab_size_to_str(cache_size size)
{
	switch (size) {
	case CACHE_8B:
		return "8B";
	case CACHE_16B:
		return "16B";
	case CACHE_32B:
		return "32B";
	case CACHE_64B:
		return "64B";
	case CACHE_128B:
		return "128B";
	case CACHE_256B:
		return "256B";
	case CACHE_512B:
		return "512B";
	case CACHE_1024B:
		return "1024B";
	case CACHE_2048B:
		return "2048B";
	default:
		return "??B";
	}
}

static size_t slab_print_zone_summary(zone_type zone)
{
	vga_printf("\n--- Slab Caches in Zone: %s ---\n", (zone == DMA_ZONE) ? "DMA" : "LOWMEM");

	size_t zone_total_mem_held = 0;
	bool   has_active_caches   = false;

	for (int i = 0; i < NUM_SLAB_CACHES; i++) {
		slab_cache_t *cache = &slab_caches[zone][i];

		size_t full_slabs    = list_count_nodes(&cache->slabs_full);
		size_t partial_slabs = list_count_nodes(&cache->slabs_partial);
		size_t empty_slabs   = list_count_nodes(&cache->slabs_empty);
		size_t total_slabs   = full_slabs + partial_slabs + empty_slabs;

		if (total_slabs == 0) {
			continue;
		}

		has_active_caches = true;

		size_t total_objects = total_slabs * cache->objects_per_slab;

		size_t            total_inuse = 0;
		struct list_head *iter;
		total_inuse += full_slabs * cache->objects_per_slab;
		for (iter = cache->slabs_partial.next; iter != &cache->slabs_partial; iter = iter->next) {
			slab_t *s = container_of(iter, slab_t, list);
			total_inuse += s->inuse;
		}

		size_t mem_held = total_slabs * PAGE_SIZE;
		zone_total_mem_held += mem_held;
		size_t mem_used = total_inuse * cache->object_size;

		uint32_t efficiency = (mem_held > 0) ? (mem_used * 100) / mem_held : 0;

		vga_printf("  Cache %s:\n", debug_slab_size_to_str(i));
		vga_printf("    Slabs: %u (Full: %u, Partial: %u, Empty: %u)\n", total_slabs, full_slabs,
		           partial_slabs, empty_slabs);
		vga_printf("    Objects: %u used of %u total (Free: %u)\n", total_inuse, total_objects,
		           total_objects - total_inuse);
		vga_printf("    Memory: %u KiB used / %u KiB held | Efficacité: %u%%\n", mem_used / 1024,
		           mem_held / 1024, efficiency);
	}

	if (!has_active_caches) {
		vga_printf("  (No active caches in this zone)\n");
	}

	return zone_total_mem_held;
}

void slab_print_summary(void)
{
	vga_printf("\n=================== Slab Allocator Summary ===================\n");

	size_t total_mem_held = 0;
	for (int zone = 0; zone < MAX_ZONE; zone++) {
		if (zone >= DMA_ZONE + 1)
			continue;
		total_mem_held += slab_print_zone_summary((zone_type)zone);
	}

	vga_printf("\n------------------------------------------------------------\n");
	vga_printf("Total Memory Held by Slab Caches: %u KiB (%u MiB)\n", total_mem_held / 1024,
	           total_mem_held / (1024 * 1024));
	vga_printf("============================================================\n");
}
