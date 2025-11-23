#include "bitmap.h"

#define BIT_IS_FULL                   255

size_t bitmap_find_first_free(const t_bitmap *map)
{
	if (!map || !map->data)
		return -1;

	size_t max = BITS_TO_BYTES(map->nb_bits);

	for (size_t i = 0; i < max; i++) {
		if (map->data[i] == BIT_IS_FULL)
			continue;
		for (size_t j = 0; j < 8; j++) {
			if (map->data[i] >> j & 1)
				continue;
			size_t index = i * 8 + j;

			if (index >= map->nb_bits)
				return (size_t)-1;

			return (index);
		}
	}
	return -1;
}
