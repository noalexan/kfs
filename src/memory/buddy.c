#include "buddy.h"
#include "boot_allocator.h"
#include "panic.h"
#include "printk.h"
#include "utils.h"
#include <libft.h>
#include "buddy.h"

/*
 * TODO : 
 *  Buddy init: parse Free Mem using bootmem to get addr_start use page_descriptor to init blocks
 *  Alloc Pages : 
 *      Split Block
 *  Free Pages :
 *      Coalescence Block
 *      Who is my buddy
 *  Utils :
 *      Remove from list
 *      Add to list
 */

buddy_allocator_t buddy[ZONE_TYPES];

void buddy_allocator_init(void)
{
    uint32_t    free_count = boot_allocator_get_region_count(FREE_MEMORY);
    region_t    *free_regions = boot_allocator_get_region(FREE_MEMORY);
    boot_allocator_freeze();

    // printk("Buddy allocator init: found %u free regions\n", free_count);
    // for (uint32_t i = 0; i < free_count; i++) {
    //     printk("  Region %u: start=0x%x\n", i, free_regions[i].start);
    // }
}
