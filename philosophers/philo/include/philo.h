/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 19:33:35 by aryamamo          #+#    #+#             */
/*   Updated: 2025/02/13 15:35:33 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

/* ************************************************************************** */
/*                              STRUCT DEFINITIONS                            */
/* ************************************************************************** */

typedef struct s_params
{
	int				num_philos;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat;
	int				must_eat_flag;
	long			start_time;
	int				simulation_end;
	pthread_mutex_t	logging;
	pthread_mutex_t	simulation_lock;
	pthread_mutex_t	*forks;
	t_philosopher	*philos;
	pthread_t		*threads;

}					t_params;

typedef struct s_philosopher
{
	int				id;
	long			last_meal;
	int				meals_eaten;
	t_params		*params;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
}					t_philosopher;

/* ************************************************************************** */
/*                              FUNCTIONS                                     */
/* ************************************************************************** */

#endif