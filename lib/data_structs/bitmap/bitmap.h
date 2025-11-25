#pragma once

#include <types.h>
#include <utils/macro.h>

typedef struct s_bitmap {
	size_t   nb_bits;
	uint8_t *data;
} t_bitmap;

#define BITS_TO_BYTES(nb_bits) DIV_ROUND_UP((nb_bits), 8)

void    bitmap_set(t_bitmap *map, size_t index);
void    bitmap_unset(t_bitmap *map, size_t index);
bool    bitmap_check(const t_bitmap *map, size_t index);
void    bitmap_clear(t_bitmap *map);
ssize_t bitmap_find_next_zero(size_t nb_bits, uint8_t *data);
ssize_t bitmap_find_next_zero_from(size_t nb_bits, uint8_t *data, size_t bit_start);
