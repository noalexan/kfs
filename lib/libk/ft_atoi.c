/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 21:56:00 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/03 22:35:02 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libk.h"

static int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\v' || c == '\n'
		|| c == '\f' || c == '\r');
}

int	ft_atoi(const char *nptr)
{
	size_t	i;
	int		sig;
	int		num;

	sig = 1;
	i = 0;
	num = 0;
	while (ft_isspace(nptr[i]))
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sig *= -1;
		i++;
	}
	while (ft_isdigit(nptr[i]))
	{
		num *= 10;
		num += nptr[i] - '0';
		i++;
	}
	return (num * sig);
}
