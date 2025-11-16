/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbettini <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/18 16:29:31 by jbettini          #+#    #+#             */
/*   Updated: 2021/10/19 18:25:07 by jbettini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	intlen(long long n)
{
	int	i;
	int	neg;

	i = 0;
	neg = 0;
	if (n == 0)
		return (1);
	else if (n < 0)
	{
		neg += 1;
		n *= -1;
	}
	while (n != 0)
	{
		i++;
		n /= 10;
	}
	return (i + neg);
}

static char	*makeit(long long n, char *str)
{
	size_t	neg;
	size_t	i;

	neg = 0;
	i = intlen(n);
	str[i] = 0;
	i -= 1;
	if (n < 0)
	{
		n *= -1;
		neg += 1;
		str[0] = '-';
	}
	else if (n == 0)
		str[0] = 48;
	while (n != 0 && i >= neg)
	{
		str[i] = (n % 10) + 48;
		n /= 10;
		i--;
	}
	return (str);
}

char	*ft_itoa(int n)
{
	char		*str;
	long long	res;

	res = n;
	str = malloc(sizeof(char) * intlen(res) + 1);
	if (!str)
		return (NULL);
	str = makeit(res, str);
	return (str);
}
