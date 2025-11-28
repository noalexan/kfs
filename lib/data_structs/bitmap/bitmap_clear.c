#include "bitmap.h"
#include <libk.h>

void bitmap_clear(t_bitmap *map)
{
	if (!map || !map->data)
		return;
	ft_bzero(map->data, BITS_TO_BYTES(map->nb_bits));
}
