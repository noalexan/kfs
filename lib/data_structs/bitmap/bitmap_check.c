#include "bitmap.h"

bool bitmap_check(const t_bitmap *map, size_t index)
{
	if (!map || !map->data || index >= map->nb_bits)
		return false;
	size_t byte_idx = index / 8;
	size_t bit_idx  = index % 8;
	return (map->data[byte_idx] >> bit_idx) & 1;
}
