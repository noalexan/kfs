#ifndef TYPES_H
#define TYPES_H

#ifndef NULL
# define NULL (void *)0
#endif

#define bool  _Bool
#define true  1
#define false 0

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned int   uintptr_t;

typedef unsigned int size_t;
typedef int          ssize_t;

struct list_head {
	struct list_head *next, *prev;
};

typedef unsigned int gfp_t;

#define __GFP_KERNEL 0b00000001 // For lazy allocation
#define __GFP_ATOMIC 0b00000010 // Usefull in futur when scheduler is OK
#define __GFP_DMA    0b00000100 // asking for dma
// #define __GFP_HIGH    	0b00001000 // asking for highmem
#define __GFP_ZERO 0b00010000 // asking to fill memory with zero

#define GFP_DMA        (__GFP_DMA | __GFP_KERNEL)
#define GFP_ATOMIC_DMA (__GFP_DMA | __GFP_ATOMIC)

// Standard flags to choose lowmem
#define GFP_KERNEL (__GFP_KERNEL)
#define GFP_ATOMIC (__GFP_ATOMIC)

#endif
