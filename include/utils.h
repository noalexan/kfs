#pragma once

#if __has_attribute(__fallthrough__)
# define fallthrough __attribute__((__fallthrough__))
#else
# define fallthrough                                                                               \
	 do {                                                                                          \
	 } while (0) /* fallthrough */
#endif // __fallthrough__

// Macro
#define ALIGN(x, a)           __ALIGN_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define DIV_ROUND_UP(x, y)    (x + y - 1) / y

// Define

#define PAGE_SIZE 4096
