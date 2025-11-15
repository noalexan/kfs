/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <jbettini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:08:51 by jbettini          #+#    #+#             */
/*   Updated: 2024/03/28 05:17:56 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*li;

	li = malloc(sizeof(t_list));
	if (!li)
		return (NULL);
	li->content = content;
	li->next = NULL;
	li->prev = NULL;
	return (li);
}

t_list  *ft_lstnew_prev(void *content, t_list *prev) {
    	t_list	*li;

	li = malloc(sizeof(t_list));
	if (!li)
		return (NULL);
	li->content = content;
	li->next = NULL;
    li->prev = prev;
	return (li);
}