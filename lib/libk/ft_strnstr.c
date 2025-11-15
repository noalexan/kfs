/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/02 22:31:51 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/07 22:34:02 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

char	*ft_strnstr(const char *str, const char *seg, size_t len)
{
	size_t	i;
	size_t	l;

	i = 0;
	if (seg[0] == 0)
		return ((char *) str);
	if (len)
	{
		while (str[i] && i < len)
		{
			l = 0;
			while (seg[l] && str[i + l] == seg[l] && i + l < len)
			{
				l++;
			}
			if (seg[l] == 0)
				return ((char *) &str[i]);
			i++;
		}
	}
	return (NULL);
}
