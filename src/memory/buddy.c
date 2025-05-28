#include "../panic.h"
#include "../printk/printk.h"
#include "utils.h"
#include <libft.h>

page_t           *page_descriptors;
buddy_allocator_t buddy[MAX_ORDER];

// TODO: get full free size + init all page descriptors

void buddy_allocator_init(void) {}