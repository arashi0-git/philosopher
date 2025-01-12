/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:44:28 by aryamamo          #+#    #+#             */
/*   Updated: 2024/11/06 11:29:51 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	unsigned char	*sp;
	unsigned char	target;

	target = (unsigned char)c;
	sp = (unsigned char *)s;
	while (n--)
	{
		if (*sp == target)
			return ((void *)sp);
		sp++;
	}
	return (NULL);
}
