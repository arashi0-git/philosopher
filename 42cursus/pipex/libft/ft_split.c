/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:14:50 by aryamamo          #+#    #+#             */
/*   Updated: 2025/01/08 12:10:38 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	**split_free(char **tab)
{
	size_t	i;

	if (!tab)
		return (NULL);
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
	return (NULL);
}

size_t	ft_count_words(char const *s, char c)
{
	size_t	count;
	int		in_word;

	count = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c && !in_word)
		{
			in_word = 1;
			count++;
		}
		else if (*s == c)
			in_word = 0;
		s++;
	}
	return (count);
}

static size_t	ft_word_len(char const *s, char c)
{
	size_t	len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	return (len);
}

char	**ft_split(char const *s, char c)
{
	char	**tab;
	size_t	i;
	size_t	j;
	size_t	word_len;

	if (!s)
		return (NULL);
	tab = ft_calloc((ft_count_words(s, c) + 1), sizeof(char *));
	if (!tab)
		return (NULL);
	i = -1;
	j = 0;
	while (s[++i])
	{
		if (s[i] != c)
		{
			word_len = ft_word_len(&s[i], c);
			(tab[j] = ft_substr(s, i, word_len));
			if (!tab[j])
				return (split_free(tab));
			i += word_len - 1;
			j++;
		}
	}
	return (tab);
}

// #include <stdio.h>

// int	main(void)
// {
// 	char	**result;
// 	char	c;

// 	c = 'b';
// 	result = ft_split("aabbcc", c);
// 	printf("%s\n", *result);
// 	return (0);
// }
