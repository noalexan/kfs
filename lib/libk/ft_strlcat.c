/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 14:03:04 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/07 23:49:19 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <types.h>

size_t	ft_strlcat(char *dst, const char *src, size_t dsize)
{
	size_t	i;
	size_t	j;
	size_t	dst_len;
	size_t	src_len;

	dst_len = 0;
	src_len = 0;
	if (src == NULL)
		return (0);
	while (dst[dst_len] != '\0' && dst_len < dsize)
		dst_len++;
	while (src[src_len] != '\0')
		src_len++;
	if (dst_len == dsize)
		return (dsize + src_len);
	i = dst_len;
	j = 0;
	while (src[j] != '\0' && i < dsize - 1)
		dst[i++] = src[j++];
	if (i < dsize)
		dst[i] = '\0';
	return (dst_len + src_len);
}
