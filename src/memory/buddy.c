#include "../gdt.h"
#include "../idt.h"
#include "../panic.h"
#include "../printk/printk.h"
#include "utils.h"
#include <libft.h>

uint8_t        *bitmap;
static uint32_t num_of_pages = 0;
static uint32_t bitmap_size  = 0;

/*
 *
 * struct multiboot_mmap_entry {
 *     uint64_t addr;   // first phy addr of the range
 *     uint64_t len;    // memory range
 *     uint32_t type;   // range type
 *     uint32_t zero;   // réserved
 * } __attribute__((packed));
 *
 *  Available range type:
 *      1	Available memory RAM
 *      2	Reserved

 *
 */

// void buddy_allocator_init(multiboot_tag_mmap_t *mmap, uint8_t *mmap_end)
// {

// }