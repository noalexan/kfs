/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 05:48:15 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/07 22:40:18 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <types.h>

void *ft_memmove(void *dest, const void *src, size_t n)
{
	size_t i;

	if (dest < src) {
		i = 0;
		while (i < n) {
			((unsigned char *)dest)[i] = ((unsigned char *)src)[i];
			i++;
		}
	} else if (dest > src)
		while (n--)
			((unsigned char *)dest)[n] = ((unsigned char *)src)[n];
	return (dest);
}
