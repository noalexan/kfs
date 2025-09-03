#include <drivers/vga.h>
#include <kernel/panic.h>
#include <memory/buddy.h>
#include <memory/kmalloc.h>
#include <memory/memory.h>
#include <memory/page.h>
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

typedef struct slab_cache {
	struct list_head slabs_full;
	struct list_head slabs_partial;
	struct list_head slabs_empty;
	size_t           object_size;
	uint32_t         objects_per_slab;

} slab_cache_t;

typedef struct slab {
	struct list_head list;
	void            *freelist;
	uint32_t         inuse;
	slab_cache_t    *parent_cache;
} slab_t;

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
		/* USing kmalloc here is really smart,
		 * but we need to keep in mind if this shitty kernel implement multi core (SMP) architecture
		 * the concurency can create race condition or at least deadlock
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
	slab_t       *slab  = page->private_data;
	slab_cache_t *cache = slab->parent_cache;

	char *obj      = ptr;
	*(void **)obj  = slab->freelist;
	slab->freelist = obj;
	slab->inuse--;
	if (slab->inuse == cache->objects_per_slab - 1) {
		pop_node(&slab->list);
		list_add_head(&slab->list, &cache->slabs_partial);
	} else if (slab->inuse == 0) {
		// TODO : implement an memory shrinker logique to simplify logic and dont implement an
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
