/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: noalexan <noalexan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/01 05:13:34 by noalexan          #+#    #+#             */
/*   Updated: 2024/06/01 13:06:33 by noalexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <types.h>

size_t ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

size_t ft_strslen(char **str)
{
	size_t i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}
