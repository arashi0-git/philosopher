/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 11:38:14 by aryamamo          #+#    #+#             */
/*   Updated: 2024/11/13 12:12:48 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <limits.h>

static int	ft_skip_and_sign(const char **nptr)
{
	int	sign;

	sign = 1;
	while (**nptr == 32 || (**nptr >= 9 && **nptr <= 13))
		(*nptr)++;
	if (**nptr == '-')
	{
		sign = -1;
		(*nptr)++;
	}
	else if (**nptr == '+')
		(*nptr)++;
	return (sign);
}

int	ft_atoi(const char *nptr)
{
	long long	result;
	int			sign;

	result = 0;
	sign = ft_skip_and_sign(&nptr);
	while (*nptr >= '0' && *nptr <= '9')
	{
		if (result > (LONG_MAX - (*nptr - '0')) / 10)
		{
			if (sign == 1)
				return ((int)LONG_MAX);
			return ((int)LONG_MIN);
		}
		result = result * 10 + (*nptr - '0');
		nptr++;
	}
	return (result * sign);
}

// #include <limits.h>
// #include <stdio.h>
// #include <stdlib.h>

// int			ft_atoi(const char *nptr);

// int	main(void)
// {
// 	int	i;

// 	const char *test_cases[] = {"0",
// 								"42",
// 								"-42",
// 								"2147483647",            // INT_MAX
// 								"-2147483648",           // INT_MIN
// 								"9223372036854775808",   // LONG_MAX
// 								"-9223372036854775808",  // LONG_MIN
// 								"9223372036854775808",   // LONG_MAX
// 								"-9223372036854775809",  // LONG_MIN
// 								"18446744073709551615",  // さらに大きい数
// 								"-18446744073709551616", // さらに小さい数
// 								NULL};
// 	i = 0;
// 	while (test_cases[i])
// 	{
// 		printf("Test case: \"%s\"\n", test_cases[i]);
// 		printf("atoi:    %d\n", atoi(test_cases[i]));
// 		printf("ft_atoi: %d\n", ft_atoi(test_cases[i]));
// 		printf("-----\n");
// 		i++;
// 	}
// 	return (0);
// }
