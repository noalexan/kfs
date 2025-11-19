#include "liblst.h"

t_list *ft_lst_pop(t_list **list)
{
    if (*list == NULL)
        return NULL;

    t_list *first = *list;
    *list = first->next;
    first->next = NULL;

    return first;
}