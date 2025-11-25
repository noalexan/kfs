#include <memory/kmalloc.h>
#include <memory/memory.h>
#include <utils/id_manager.h>

t_id_manager *id_manager_create(size_t max_id)
{
	size_t nb_of_bytes = BITS_TO_BYTES(max_id);

	t_id_manager *ret = kmalloc(sizeof(t_id_manager) + nb_of_bytes, GFP_KERNEL);
	if (!ret)
		return NULL;

	ret->bitmap.data    = (uint8_t *)(ret + 1);
	ret->bitmap.nb_bits = max_id;
	ret->next_free      = 0;

	bitmap_clear(&ret->bitmap);

	return ret;
}

void id_manager_destroy(t_id_manager *mgr)
{
	if (mgr)
		kfree(mgr);
}

ssize_t id_manager_alloc(t_id_manager *mgr)
{
	ssize_t ret = mgr->next_free;
	if (ret == -1)
		return ret;
	size_t index = BITS_TO_BYTES(ret);
	bitmap_set(&mgr->bitmap, ret);
	if (mgr->bitmap.data[index] == 0xFF) {
		mgr->next_free = bitmap_find_first_free(mgr->bitmap.nb_bits, &mgr->bitmap.data[index + 1]);
		if (mgr->next_free == -1)
			mgr->next_free = bitmap_find_first_free(mgr->bitmap.nb_bits, &mgr->bitmap.data);
	} else {
		uint8_t bit = ret;
		while (bit < 8) {
			if (mgr->bitmap.data[index] >> bit & 1)
				continue;
			size_t new_value = index * 8 + bit;
			if (new_value >= mgr->bitmap.nb_bits)
				return -1;
			mgr->next_free = new_value;
		}
	}
	return ret;
}
