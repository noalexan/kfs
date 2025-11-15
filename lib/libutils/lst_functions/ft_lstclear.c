/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 16:33:01 by jbettini          #+#    #+#             */
/*   Updated: 2024/03/25 06:16:38 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	noFree(void *null) {
	(void)null;
}

void	ft_lstclear(t_list **alst, void (*del)(void *))
{
	t_list	*li;

	li = *alst;
	if (alst)
	{
		if (*alst)
		{
			while (li)
			{
				li = li->next;
				(*del)((*alst)->content);
				free((void *)*alst);
				*alst = 0;
				*alst = li;
			}
		}
	}
}
