/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 15:16:57 by aryamamo          #+#    #+#             */
/*   Updated: 2024/11/08 16:25:27 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	const unsigned char	*x;
	unsigned char		*y;
	size_t				count;

	count = 0;
	x = (const unsigned char *)src;
	y = (unsigned char *)dest;
	while (count < n)
	{
		y[count] = x[count];
		count++;
	}
	return (dest);
}
