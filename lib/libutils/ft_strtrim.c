/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 18:45:34 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/21 17:17:15 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	is_sep(char c, const char *charset)
{
	size_t	i;

	i = 0;
	while (charset[i])
	{
		if (c == charset[i++])
			return (1);
	}
	return (0);
}

static size_t	end_charset(char const *s, const char *charset)
{
	size_t	j;
	size_t	i;

	i = 0;
	j = ft_strlen(s) - 1;
	while (is_sep(s[j], charset))
	{
		j--;
		i++;
	}
	return (i);
}

char	*ft_strtrim(char const *s, char const *charset)
{
	size_t	i;
	size_t	j;
	size_t	k;
	char	*str;

	if (!s)
		return (NULL);
	i = 0;
	j = ft_strlen(s);
	k = end_charset(s, charset);
	while (is_sep(s[i], charset))
		i++;
	if (i == j)
		k = 0;
	str = malloc(sizeof(char) * (j - (k + i)) + 1);
	if (!str)
		return (NULL);
	j = 0;
	while (i < ft_strlen(s) - k)
		str[j++] = s[i++];
	str[j] = 0;
	return (str);
}
