/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:07:17 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/21 17:16:47 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*li;
	t_list	*tmp;

	tmp = NULL;
	li = NULL;
	if (lst)
	{
		while (lst)
		{
			tmp = ft_lstnew((*f)(lst->content));
			if (!tmp)
			{
				ft_lstclear(&li, del);
				ft_lstclear(&tmp, del);
				return (NULL);
			}
			ft_lstadd_back(&li, tmp);
			lst = lst->next;
		}
	}
	else
		return (NULL);
	return (li);
}
