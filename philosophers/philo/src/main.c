/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 12:45:16 by aryamamo          #+#    #+#             */
/*   Updated: 2025/02/11 13:01:31 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

int	arg_content(char *av)
{
	int	i;

	i = 0;
	while (av[i] != '\0')
	{
		if (av[i] < '0' || av[i] > '9')
			return (1);
		i++;
	}
	return (0);
}

int	validate_args(char **av)
{
	if (ft_atoi(av[1]) <= 0 || arg_content(av[1]) == 1)
		return (write(2, "Invalid philosophers number\n", 29), 1);
	if (ft_atoi(av[2]) <= 0 || arg_content(av[2]) == 1)
		return (write(2, "Invalid time to die\n", 21), 1);
	if (ft_atoi(av[3]) <= 0 || arg_content(av[3]) == 1)
		return (write(2, "Invalid time to eat\n", 21), 1);
	if (ft_atoi(av[4]) <= 0 || arg_content(av[4]) == 1)
		return (write(2, "Invalid time to sleep\n", 23), 1);
	if (av[5] && (ft_atoi(av[5]) < 0 || arg_content(av[5]) == 1))
		return (write(2, "Invalid number of times each philosopher must eat\n",
				51), 1);
}

int	main(int ac, char **argv)
{
	t_program program;
	t_philo philos[ft_atoi(argv[1])];
	pthread_mutex_t forks[ft_atoi(argv[1])];

	if (ac < 5 && ac > 6)
	{
		write(2, "Wrong arguments\n", 22);
		return (1);
	}
	if (validate_args(argv) == 1)
		return (1);
	init_program(&program, philos);
	init_forks(&forks, ft_atoi(argv[1]));
	init_philos(&philos, &program, forks, argv);
	thread_creat(&program, forks);
	destroy_all(NULL, &program, forks);
	return (0);
}