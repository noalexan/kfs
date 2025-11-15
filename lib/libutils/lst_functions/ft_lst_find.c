/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lst_find.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rahmed <rahmed@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/08 01:39:22 by jbettini          #+#    #+#             */
/*   Updated: 2022/02/09 20:48:46 by rahmed           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lst_find(t_list *lst, void *data_ref)
{
	if (!lst || !data_ref)
		return (NULL);
	while (lst && !ft_strnstr(lst->content, data_ref, ft_strlen(lst->content)))
		lst = lst->next;
	return (lst);
}

t_list	*ft_lst_find_before(t_list *lst, void *data_ref)
{
	if (!lst || !data_ref)
		return (NULL);
	if (!ft_strnstr(lst->content, data_ref, ft_strlen(lst->content)))
		while (lst && !ft_strnstr(lst->next->content, data_ref, \
		ft_strlen(lst->next->content)))
			lst = lst->next;
	else
		return (NULL);
	return (lst);
}
