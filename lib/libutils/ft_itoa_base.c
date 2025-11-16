/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoadr_base.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/26 20:33:51 by jbettini          #+#    #+#             */
/*   Updated: 2021/11/19 22:28:15 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_countlen(unsigned long long nb, char *base)
{
	size_t	i;

	i = 0;
	if (nb == 0)
		return (1);
	while (nb != 0)
	{
		nb /= ft_strlen(base);
		i++;
	}
	return (i);
}

char	*makebase(unsigned long long nb, char *base, char *str)
{
	size_t	i;

	i = ft_countlen(nb, base);
	str[i--] = 0;
	if (nb == 0)
		str[i] = 48;
	else
	{
		while (nb != 0)
		{
			str[i] = base[nb % ft_strlen(base)];
			nb /= ft_strlen(base);
			i--;
		}
	}
	return (str);
}

char	*ft_itoa_base(unsigned long long nb, char *base)
{
	char	*str;

	str = malloc(sizeof(char) * ft_countlen(nb, base) + 1);
	if (!str)
		return (NULL);
	str = makebase(nb, base, str);
	return (str);
}
