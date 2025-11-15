/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memset.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 05:21:21 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/07 22:34:46 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <types.h>

void	*ft_memset(void *s, int c, size_t n)
{
	while (n--)
		((unsigned char *) s)[n] = (unsigned char) c;
	return (s);
}
