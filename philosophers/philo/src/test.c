/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 13:27:28 by aryamamo          #+#    #+#             */
/*   Updated: 2025/02/13 13:29:13 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* ************************************************************************** */
/*                              STRUCT DEFINITIONS                            */
/* ************************************************************************** */

typedef struct s_params			t_params;
typedef struct s_philosopher	t_philosopher;

struct							s_params
{
	int num_philos;     // number of philosophers (and forks)
	long time_to_die;   // in ms
	long time_to_eat;   // in ms
	long time_to_sleep; // in ms
	int							must_eat;

	int							must_eat_flag;
	long start_time;         // simulation start time in ms
	int simulation_end;      // flag: 1 when simulation stops
	pthread_mutex_t logging; // mutex to protect printing
	pthread_mutex_t				simulation_lock;
	pthread_mutex_t *forks; // array of mutexes (one per fork)
};

struct							s_philosopher
{
	int id; // philosopher number (1-indexed)
	long						last_meal;
	int meals_eaten;             // counter for number of meals eaten
	t_params *params;            // pointer to shared simulation parameters
	pthread_mutex_t *left_fork;  // pointer to the left fork (mutex)
	pthread_mutex_t *right_fork; // pointer to the right fork (mutex)
};

/* ************************************************************************** */
/*                              UTILITY FUNCTIONS                             */
/* ************************************************************************** */

// Returns current timestamp in ms.
long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Returns timestamp relative to simulation start.
long	get_timestamp(t_params *params)
{
	return (get_time_ms() - params->start_time);
}

// Logs a philosopher’s action with a timestamp.
// The logging mutex ensures messages are not mixed up.
void	log_action(t_philosopher *phil, const char *action)
{
	pthread_mutex_lock(&phil->params->logging);
	// Only print if simulation is still running.
	if (!phil->params->simulation_end)
		printf("%ld %d %s\n", get_timestamp(phil->params), phil->id, action);
	pthread_mutex_unlock(&phil->params->logging);
}

// Sleeps for the given number of milliseconds.
void	msleep(long ms)
{
	long	start;

	start = get_time_ms();
	while (get_time_ms() - start < ms)
		usleep(100); // sleep for 100 microseconds to avoid busy waiting
}

/* ************************************************************************** */
/*                          PHILOSOPHER THREAD ROUTINE                        */
/* ************************************************************************** */

void	*philosopher_routine(void *arg)
{
	t_philosopher	*phil;
	t_params		*params;

	phil = (t_philosopher *)arg;
	params = phil->params;
	// Special case: if there is only one philosopher,
	if (params->num_philos == 1)
	{
		pthread_mutex_lock(phil->left_fork);
		log_action(phil, "has taken a fork");
		msleep(params->time_to_die);
		// he will starve waiting for the second fork.
		pthread_mutex_unlock(phil->left_fork);
		return (NULL);
	}
	// Stagger start for even-numbered philosophers to help avoid deadlocks.
	if (phil->id % 2 == 0)
		usleep(1000);
	while (1)
	{
		// Check if simulation has ended.
		pthread_mutex_lock(&params->simulation_lock);
		if (params->simulation_end)
		{
			pthread_mutex_unlock(&params->simulation_lock);
			break ;
		}
		pthread_mutex_unlock(&params->simulation_lock);
		/* --------------------------- TAKE FORKS --------------------------- */
		// To avoid deadlock,
		if (phil->id % 2 == 0)
		{
			pthread_mutex_lock(phil->right_fork);
			log_action(phil, "has taken a fork");
			pthread_mutex_lock(phil->left_fork);
			log_action(phil, "has taken a fork");
		}
		else
		{
			pthread_mutex_lock(phil->left_fork);
			log_action(phil, "has taken a fork");
			pthread_mutex_lock(phil->right_fork);
			log_action(phil, "has taken a fork");
		}
		/* ---------------------------- EATING ------------------------------ */
		pthread_mutex_lock(&params->simulation_lock);
		phil->last_meal = get_time_ms();
		pthread_mutex_unlock(&params->simulation_lock);
		log_action(phil, "is eating");
		msleep(params->time_to_eat);
		phil->meals_eaten++;
		// Release the forks after eating.
		pthread_mutex_unlock(phil->left_fork);
		pthread_mutex_unlock(phil->right_fork);
		// If an optional number of meals is provided and reached, exit.
		if (params->must_eat_flag && (phil->meals_eaten >= params->must_eat))
			break ;
		/* --------------------------- SLEEPING ----------------------------- */
		log_action(phil, "is sleeping");
		msleep(params->time_to_sleep);
		/* -------------------------- THINKING ------------------------------ */
		log_action(phil, "is thinking");
	}
	return (NULL);
}

/* ************************************************************************** */
/*                         MONITOR THREAD ROUTINE                             */
/* ************************************************************************** */

// This thread continuously checks whether any philosopher has starved
// or if all philosophers have eaten enough (when the optional argument is provided).
void	*monitor_routine(void *arg)
{
	t_philosopher	*phils;
	t_params		*params;
	int				i;
	long			time_since_last_meal;
	int				all_done;

	phils = (t_philosopher *)arg;
	params = phils[0].params;
	while (1)
	{
		for (i = 0; i < params->num_philos; i++)
		{
			pthread_mutex_lock(&params->simulation_lock);
			time_since_last_meal = get_time_ms() - phils[i].last_meal;
			if (!params->simulation_end
				&& time_since_last_meal > params->time_to_die)
			{
				// Log death – ensure this is printed within 10ms of the event.
				log_action(&phils[i], "died");
				params->simulation_end = 1;
				pthread_mutex_unlock(&params->simulation_lock);
				return (NULL);
			}
			pthread_mutex_unlock(&params->simulation_lock);
		}
		// Check if all philosophers have eaten enough times.
		if (params->must_eat_flag)
		{
			all_done = 1;
			for (i = 0; i < params->num_philos; i++)
			{
				if (phils[i].meals_eaten < params->must_eat)
				{
					all_done = 0;
					break ;
				}
			}
			if (all_done)
			{
				pthread_mutex_lock(&params->simulation_lock);
				params->simulation_end = 1;
				pthread_mutex_unlock(&params->simulation_lock);
				return (NULL);
			}
		}
		usleep(1000); // Check every 1ms
	}
	return (NULL);
}

/* ************************************************************************** */
/*                         SIMPLE atoi FUNCTION                               */
/* ************************************************************************** */

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

/* ************************************************************************** */
/*                                  MAIN                                      */
/* ************************************************************************** */

int	main(int argc, char **argv)
{
	int				i;
	t_params		params;
	t_philosopher	*philos;
	pthread_t		*threads;
	pthread_t		monitor;

	if (argc != 5 && argc != 6)
	{
		printf("time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}
	/* ---------------------- PARSE ARGUMENTS ---------------------- */
	params.num_philos = ft_atoi(argv[1]);
	params.time_to_die = ft_atoi(argv[2]);
	params.time_to_eat = ft_atoi(argv[3]);
	params.time_to_sleep = ft_atoi(argv[4]);
	params.must_eat_flag = 0;
	if (argc == 6)
	{
		params.must_eat = ft_atoi(argv[5]);
		params.must_eat_flag = 1;
	}
	params.simulation_end = 0;
	params.start_time = get_time_ms();
	pthread_mutex_init(&params.logging, NULL);
	pthread_mutex_init(&params.simulation_lock, NULL);
	/* -------------------- INITIALIZE FORKS ----------------------- */
	params.forks = malloc(sizeof(pthread_mutex_t) * params.num_philos);
	if (!params.forks)
		return (1);
	for (i = 0; i < params.num_philos; i++)
		pthread_mutex_init(&params.forks[i], NULL);
	/* -------------------- INITIALIZE PHILOSOPHERS ------------------ */
	philos = malloc(sizeof(t_philosopher) * params.num_philos);
	threads = malloc(sizeof(pthread_t) * params.num_philos);
	if (!philos || !threads)
		return (1);
	i = 0;
	while (i < params.num_philos)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].params = &params;
		philos[i].last_meal = params.start_time;
		// In this circular table, each philosopher’s left fork is forks[i] and
		// the right fork is forks[(i + 1) % num_philos].
		philos[i].left_fork = &params.forks[i];
		philos[i].right_fork = &params.forks[(i + 1) % params.num_philos];
		if (pthread_create(&threads[i], NULL, philosopher_routine,
				&philos[i]) != 0)
		{
			perror("pthread_create");
			return (1);
		}
		i++;
	}
	/* --------------------- CREATE MONITOR THREAD --------------------- */
	if (pthread_create(&monitor, NULL, monitor_routine, philos) != 0)
	{
		perror("pthread_create");
		return (1);
	}
	/* ---------------------- WAIT FOR THREADS ------------------------- */
	pthread_join(monitor, NULL);
	for (i = 0; i < params.num_philos; i++)
		pthread_join(threads[i], NULL);
	/* ------------------------- CLEAN UP ------------------------------ */
	i = 0;
	while (i < params.num_philos)
	{
		pthread_mutex_destroy(&params.forks[i]);
		i++;
	}
	free(params.forks);
	free(philos);
	free(threads);
	pthread_mutex_destroy(&params.logging);
	pthread_mutex_destroy(&params.simulation_lock);
	return (0);
}
