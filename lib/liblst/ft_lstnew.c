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

#include "liblst.h"

t_list *ft_lstnew(void *content)
{
	t_list *li;

	li = kmalloc(sizeof(t_list), GFP_KERNEL);
	if (!li)
		return (NULL);
	li->content = content;
	li->next    = NULL;
	return (li);
}