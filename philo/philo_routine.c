/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 03:11:48 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/14 03:11:50 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./philo.h"

void	ft_print_mutex(char *str, long id, t_rule *rules)
{
	printf("%lu %lu %s",ft_get_time() - rules->time_start, id, str);
}

int	ft_get_fork(t_rule *rules, long id)
{
	pthread_mutex_lock(rules->philos[id].left_fork);
	if (ft_dead(rules, id))
	{
		pthread_mutex_unlock(rules->philos[id].left_fork);
		return (1);
	}
	pthread_mutex_lock(rules->philos[id].right_fork);
	ft_print_mutex("has taken a fork", id, rules);
	return (0);
}

void	ft_eat(t_rule *rules, long id)
{
	if (ft_get_fork(rules, id))
		return ;
	rules->philos[id].last_meal = ft_get_time();
	rules->philos[id].how_much_eat++;
	ft_print_mutex("eating", id, rules);
	ft_usleep(rules, id, rules->time_to_eat);
}

int	ft_eating(t_rule *rules, long id)
{
	if (ft_dead(rules, id))
		return (1);
	if (rules->philos[id].how_much_eat == rules->number_of_meals)
		usleep(500);
	ft_eat(rules, id);
	return (0);
}

void	ft_usleep(t_rule *rules, long id, long time)
{
	long	start_action;
	long	target;

	if (ft_dead(rules, id))
		return ;
	start_action = ft_get_time();
	while (start_action - ft_get_time() < time)
	{
		if (!ft_dead(rules, id))
			usleep(500);
		else
			break ;
	}
}
int	ft_sleeping(t_rule *rules, long id)
{
	if (ft_dead(rules, id))
		return (1);
	ft_print_mutex("sleeping", id, rules);
	ft_usleep(rules, id, rules->time_to_sleep);
	return (0);
}

int	ft_dead(t_rule *rules, long id)
{
	long	elapsed;

	elapsed = (ft_get_time() - rules->time_start) - rules->philos[id].last_meal;
	if (elapsed > rules->time_to_die)
	{
		// Print morte monitor
		rules->philos[id].is_dead = 1;
		return (1);
	}
	return (0);
}
