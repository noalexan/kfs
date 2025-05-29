#include "buddy.h"
#include "boot_allocator.h"
#include "panic.h"
#include "printk.h"
#include "utils.h"
#include <libft.h>

buddy_allocator_t buddy[MAX_ORDER];

// TODO: get full free size + init all page descriptors

void buddy_allocator_init(void) {}