#pragma once

#include <types.h>
#include <utils.h>

typedef struct s_bitmap {
	size_t   nb_bits;
	uint8_t *data;
} t_bitmap;

#define BITS_TO_BYTES(nb_bits) DIV_ROUND_UP((nb_bits), 8)

void   bitmap_set(t_bitmap *map, size_t index);
void   bitmap_unset(t_bitmap *map, size_t index);
bool   bitmap_check(const t_bitmap *map, size_t index);
void   bitmap_clear(t_bitmap *map);
size_t bitmap_find_first_free(const t_bitmap *map);
