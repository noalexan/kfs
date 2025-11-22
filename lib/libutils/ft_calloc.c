/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 16:21:36 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/21 17:08:41 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libutils.h"

void *ft_calloc(size_t count, size_t size)
{
	void *res;

	res = kmalloc(count * size, GFP_KERNEL | __GFP_ZERO);
	if (!res)
		return (NULL);
	return (res);
}
