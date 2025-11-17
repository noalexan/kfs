#include "liblst.h"

void	ft_lstadd_back(t_list **alst, t_list *new)
{
	t_list	*li;

	li = *alst;
	if (alst)
	{
		if (*alst)
		{
			while (li->next)
				li = li->next;
			li->next = new;
		}
		else {
			*alst = new;
		}
	}
}
