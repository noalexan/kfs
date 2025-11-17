/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strmapi.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:41:04 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/19 13:58:24 by jbettini         ###   ########.fr       */
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

char	*ft_strmapi(char const *s, char (*f)(unsigned int, char))
{
	size_t	i;
	char	*str;

	if (!s)
		return (NULL);
	i = -1;
	str = kmalloc(sizeof(char) * ft_strlen(s) + 1, GFP_KERNEL);
	if (!str)
		return (NULL);
	while (s[++i])
		str[i] = (*f)(i, s[i]);
	str[i] = 0;
	return (str);
}
