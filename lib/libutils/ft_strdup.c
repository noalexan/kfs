/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:33:24 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/18 18:33:45 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libutils.h"

static size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *src)
{
	char	*str;
	size_t	i;

	i = 0;
	str = kmalloc(sizeof(char) * ft_strlen(src) + 1, GFP_KERNEL);
	if (!str)
		return (NULL);
	while (src[i])
	{
		str[i] = src[i];
		i++;
	}
	str[i] = '\0';
	return (str);
}
