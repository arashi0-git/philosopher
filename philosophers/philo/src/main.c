/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/19 12:45:16 by aryamamo          #+#    #+#             */
/*   Updated: 2025/02/13 17:53:57 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "../include/philo.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* ************************************************************************** */
/*                              STRUCT DEFINITIONS                            */
/* ************************************************************************** */
typedef struct s_params			t_params;
typedef struct s_philosopher	t_philosopher;

struct							s_philosopher
{
	int							id;
	long						last_meal;
	int							meals_eaten;
	t_params					*params;
	pthread_mutex_t				*left_fork;
	pthread_mutex_t				*right_fork;
};

struct							s_params
{
	int							num_philos;
	long						time_to_die;
	long						time_to_eat;
	long						time_to_sleep;
	int							must_eat;
	int							must_eat_flag;
	long						start_time;
	int							simulation_end;
	pthread_mutex_t				logging;
	pthread_mutex_t				simulation_lock;
	pthread_mutex_t				*forks;
	t_philosopher				*philos;
	pthread_t					*threads;
};

void							*philosopher_routine(void *arg);
long							get_time_ms(void);

int	ft_atoi(const char *str)
{
	int		sign;
	long	result;

	sign = 1;
	result = 0;
	while (*str && (*str == ' ' || (*str >= 9 && *str <= 13)))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str && (*str >= '0' && *str <= '9'))
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (int)(result * sign);
}

int	printf_error_msg(char *msg)
{
	write(STDERR_FILENO, msg, strlen(msg));
	write(1, "\n", 1);
	return (1);
}

int	validate_arguments(int argc, char **argv)
{
	if (argc != 5 && argc != 6)
		return (printf_error_msg("number of arguments is invalid"));
	if (ft_atoi(argv[1]) > 200 || ft_atoi(argv[1]) <= 0)
		return (printf_error_msg("number of philosopher is invalid"));
	if (ft_atoi(argv[2]) <= 0)
		return (printf_error_msg("time to die is invalid"));
	if (ft_atoi(argv[3]) <= 0)
		return (printf_error_msg("time to eat is invalid"));
	if (ft_atoi(argv[4]) <= 0)
		return (printf_error_msg("time to sleep is invalid"));
	if (argc == 6)
	{
		if (ft_atoi(argv[5]) <= 0)
			return (printf_error_msg("number of eating is invalid"));
	}
	return (0);
}

int	create_philos_thread(t_philosopher *philos, pthread_t *threads,
		t_params *params)
{
	int	i;

	i = 0;
	while (i < params->num_philos)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].params = params;
		philos[i].left_fork = &params->forks[i];
		philos[i].right_fork = &params->forks[(i + 1) % params->num_philos];
		philos[i].last_meal = params->start_time;
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				&philos[i]) != 0)
		{
			perror("pthread_create");
			return (1);
		}
		i++;
	}
	return (0);
}

int	init_philos(t_params *params)
{
	int	ret;

	params->philos = malloc(sizeof(t_philosopher) * params->num_philos);
	if (!params->philos)
	{
		write(STDERR_FILENO, "philos malloc failed\n", 21);
		return (1);
	}
	params->threads = malloc(sizeof(pthread_t) * params->num_philos);
	if (!params->threads)
	{
		write(STDERR_FILENO, "threads malloc failed\n", 22);
		free(params->philos);
		return (1);
	}
	ret = create_philos_thread(params->philos, params->threads, params);
	if (ret != 0)
	{
		free(params->philos);
		free(params->threads);
		return (1);
	}
	return (0);
}

int	init_forks(t_params *params)
{
	int	i;

	i = 0;
	params->forks = malloc(sizeof(pthread_mutex_t) * params->num_philos);
	if (!params->forks)
	{
		write(STDERR_FILENO, "forks malloc failed\n", 20);
		return (1);
	}
	while (i < params->num_philos)
	{
		pthread_mutex_init(&params->forks[i], NULL);
		i++;
	}
	return (0);
}

t_params	init_params(int argc, char **argv)
{
	t_params	params;

	params.num_philos = ft_atoi(argv[1]);
	params.time_to_die = ft_atoi(argv[2]);
	params.time_to_eat = ft_atoi(argv[3]);
	params.time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
	{
		params.must_eat = ft_atoi(argv[5]);
		params.must_eat_flag = 1;
	}
	else
	{
		params.must_eat_flag = 0;
		params.must_eat = 0;
	}
	params.simulation_end = 0;
	params.start_time = get_time_ms();
	pthread_mutex_init(&params.logging, NULL);
	pthread_mutex_init(&params.simulation_lock, NULL);
	return (params);
}

void	cleanup_mutex(t_params *params)
{
	int	i;

	i = 0;
	while (i < params->num_philos)
	{
		pthread_mutex_destroy(&params->forks[i]);
		i++;
	}
	free(params->forks);
	free(params->philos);
	free(params->threads);
	pthread_mutex_destroy(&params->logging);
	pthread_mutex_destroy(&params->simulation_lock);
}

/* ************************************************************************** */
/*                              UTILITY FUNCTIONS                             */
/* ************************************************************************** */

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

long	get_timestamp(t_params *params)
{
	return (get_time_ms() - params->start_time);
}

void	log_action(t_philosopher *philo, const char *action)
{
	pthread_mutex_lock(&philo->params->logging);
	if (!philo->params->simulation_end)
		printf("%ld %d %s\n", get_timestamp(philo->params), philo->id, action);
	pthread_mutex_unlock(&philo->params->logging);
}

void	msleep(long ms)
{
	long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
		usleep(100);
}

/* ************************************************************************** */
/*                          PHILOSOPHER THREAD ROUTINE                        */
/* ************************************************************************** */

int	check_is_dead(t_params *params)
{
	int	dead;

	pthread_mutex_lock(&params->simulation_lock);
	dead = params->simulation_end;
	pthread_mutex_unlock(&params->simulation_lock);
	return (dead);
}

void	take_forks(t_philosopher *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->right_fork);
		log_action(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_fork);
		log_action(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->left_fork);
		log_action(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		log_action(philo, "has taken a fork");
	}
}

void	eat_philo(t_params *params, t_philosopher *philo)
{
	pthread_mutex_lock(&params->simulation_lock);
	philo->last_meal = get_time_ms();
	pthread_mutex_unlock(&params->simulation_lock);
	log_action(philo, "is eating");
	msleep(params->time_to_eat);
	philo->meals_eaten++;
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void	sleep_philo(t_params *params, t_philosopher *philo)
{
	log_action(philo, "is sleeping");
	msleep(params->time_to_sleep);
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	t_params		*params;

	philo = (t_philosopher *)arg;
	params = philo->params;
	if (params->num_philos == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		log_action(philo, "has taken a fork");
		msleep(params->time_to_die);
		pthread_mutex_unlock(philo->left_fork);
		return (NULL);
	}
	if (philo->id % 2 == 0)
		usleep(1000);
	while (1)
	{
		if (check_is_dead(params))
			break ;
		take_forks(philo);
		eat_philo(params, philo);
		sleep_philo(params, philo);
		log_action(philo, "is thinking");
	}
	return (NULL);
}

int	stop_if_dead(t_params *params, t_philosopher *philo)
{
	long	time_since_last_meal;
	int		died;

	died = 0;
	pthread_mutex_lock(&params->simulation_lock);
	time_since_last_meal = get_time_ms() - philo->last_meal;
	if (!params->simulation_end && time_since_last_meal > params->time_to_die)
	{
		log_action(philo, "died");
		params->simulation_end = 1;
		died = 1;
	}
	pthread_mutex_unlock(&params->simulation_lock);
	return (died);
}

int	stop_by_eat_times(t_params *params, t_philosopher *philos)
{
	int	i;
	int	all_done;

	i = 0;
	all_done = 1;
	while (i < params->num_philos)
	{
		if (philos[i].meals_eaten < params->must_eat)
		{
			all_done = 0;
			break ;
		}
		i++;
	}
	if (all_done)
	{
		pthread_mutex_lock(&params->simulation_lock);
		params->simulation_end = 1;
		pthread_mutex_unlock(&params->simulation_lock);
		return (1);
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_philosopher	*philos;
	t_params		*params;
	int				i;

	philos = (t_philosopher *)arg;
	params = philos[0].params;
	while (1)
	{
		i = 0;
		while (i < params->num_philos)
		{
			if (stop_if_dead(params, &philos[i]))
				return (NULL);
			i++;
		}
		if (params->must_eat_flag)
		{
			if (stop_by_eat_times(params, philos))
				return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

int	main(int argc, char **argv)
{
	t_params	params;
	pthread_t	monitor;
	int			i;

	i = 0;
	if (validate_arguments(argc, argv) != 0)
		return (1);
	params = init_params(argc, argv);
	if (init_forks(&params) != 0)
		return (1);
	if (init_philos(&params) != 0)
		return (1);
	if (pthread_create(&monitor, NULL, monitor_routine, params.philos) != 0)
	{
		perror("pthread_create");
		return (1);
	}
	pthread_join(monitor, NULL);
	while (i < params.num_philos)
		pthread_join(params.threads[i++], NULL);
	cleanup_mutex(&params);
	return (0);
}
