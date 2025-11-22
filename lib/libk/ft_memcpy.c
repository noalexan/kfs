/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 05:26:19 by noalexan          #+#    #+#             */
/*   Updated: 2025/01/26 17:41:09 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <types.h>

void *ft_memcpy(void *dest, const void *src, size_t n)
{
	char       *d;
	const char *s;

	s = src;
	d = dest;
	while (n--)
		*d++ = *s++;
	return (dest);
}
