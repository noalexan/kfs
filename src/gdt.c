#include <gdt.h>
#include <libft.h>

void init_gdt()
{
	ft_bzero(GDT_ENTRY(0), sizeof(gdt_entry));
}
