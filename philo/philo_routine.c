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
	pthread_mutex_lock(&rules->print_lock);
	if (!ft_is_dead(rules))
		printf("%ld %ld %s\n", ft_get_time() - rules->time_start, id + 1, str);
	pthread_mutex_unlock(&rules->print_lock);
}

int	ft_get_fork(t_rule *rules, long id)
{
	pthread_mutex_t	*first;
	pthread_mutex_t	*second;

	first = rules->philos[id].left_fork;
	second = rules->philos[id].right_fork;
	if (id % 2 == 0)
	{
		first = rules->philos[id].right_fork;
		second = rules->philos[id].left_fork;
	}
	pthread_mutex_lock(first);
	if (ft_is_dead(rules))
	{
		pthread_mutex_unlock(first);
		return (1);
	}
	pthread_mutex_lock(second);
	ft_print_mutex("has taken a fork", id, rules);
	return (0);
}

int	ft_put_fork(t_rule *rules, long id)
{
	pthread_mutex_unlock(rules->philos[id].left_fork);
	pthread_mutex_unlock(rules->philos[id].right_fork);
	return (0);
}

void	ft_eat(t_rule *rules, long id)
{
	if (ft_get_fork(rules, id))
		return ;
	pthread_mutex_lock(&rules->state_lock);
	rules->philos[id].last_meal = ft_get_time();
	pthread_mutex_unlock(&rules->state_lock);
	ft_print_mutex("is eating", id, rules);
	ft_usleep(rules, rules->time_to_eat);
	pthread_mutex_lock(&rules->state_lock);
	rules->philos[id].how_much_eat++;
	pthread_mutex_unlock(&rules->state_lock);
	ft_put_fork(rules, id);
}

void	ft_usleep(t_rule *rules, long time)
{
	long	start;

	start = ft_get_time();
	while (ft_get_time() - start < time)
	{
		if (ft_is_dead(rules))
			break ;
		usleep(200);
	}
}
