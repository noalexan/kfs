#include "bitmap.h"

void bitmap_set(t_bitmap *map, size_t index)
{
	if (!map || !map->data || index >= map->nb_bits)
		return;
	size_t  byte_idx = index / 8;
	uint8_t mask     = 1 << (index % 8);
	map->data[byte_idx] |= mask;
}
