/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:46:46 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/19 14:00:03 by jbettini         ###   ########.fr       */
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

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*str;
	size_t	i;

	if (!s)
		return (NULL);
	if (start >= ft_strlen(s))
		return (ft_calloc(1, 1));
	if (len > ft_strlen(s))
		return (ft_strdup(&s[start]));
	str = kmalloc(sizeof(char) * len + 1, GFP_KERNEL);
	if (!str)
		return (NULL);
	i = 0;
	while (i < len && s[start])
		str[i++] = s[start++];
	str[i] = '\0';
	return (str);
}
