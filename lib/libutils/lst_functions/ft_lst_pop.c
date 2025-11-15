#include "libft.h"

void ft_lst_pop(t_list **list) {
    if (*list == NULL)
        return;

    t_list *first = *list;
    *list = first->next;

    free(first);
}