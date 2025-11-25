#include "bitmap.h"

#define BIT_IS_FULL                   255

ssize_t bitmap_find_first_free(size_t nb_bits, uint8_t *data)
{
	if (!data)
		return -1;

	size_t max = BITS_TO_BYTES(nb_bits);

	for (size_t i = 0; i < max; i++) {
		if (data[i] == BIT_IS_FULL)
			continue;
		for (size_t j = 0; j < 8; j++) {
			if (data[i] >> j & 1)
				continue;
			size_t index = i * 8 + j;

			if (index >= nb_bits)
				return -1;

			return (ssize_t)index;
		}
	}
	return -1;
}

ssize_t bitmap_find_first_free_from(size_t nb_bits, uint8_t *data, size_t bit_start)
{
	if (!data)
		return -1;

	size_t max       = BITS_TO_BYTES(nb_bits);
	size_t start_idx = BITS_TO_BYTES(bit_start);
	for (size_t i = start_idx; i < max; i++) {
		if (data[i] == BIT_IS_FULL)
			continue;
		for (size_t j = 0; j < 8; j++) {
			if (data[i] >> j & 1)
				continue;
			size_t index = i * 8 + j;

			if (index >= nb_bits)
				return -1;

			return (ssize_t)index;
		}
	}
	return -1;
}
