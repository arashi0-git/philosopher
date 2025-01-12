/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:54:35 by aryamamo          #+#    #+#             */
/*   Updated: 2024/11/07 14:56:43 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	get_digit_count(int n)
{
	size_t	count;

	count = 0;
	if (n <= 0)
		count = 1;
	while (n != 0)
	{
		n /= 10;
		count++;
	}
	return (count);
}

char	*ft_itoa(int n)
{
	size_t			len;
	char			*result;
	unsigned int	num;

	len = get_digit_count(n);
	result = (char *)malloc(len + 1);
	if (result == NULL)
		return (NULL);
	if (n < 0)
	{
		result[0] = '-';
		num = -n;
	}
	else
		num = n;
	if (num == 0)
		result[0] = '0';
	result[len] = '\0';
	while (num != 0)
	{
		result[len - 1] = (num % 10) + '0';
		num = num / 10;
		len--;
	}
	return (result);
}
