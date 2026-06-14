/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 12:00:00 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/14 12:00:00 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_is_dead(t_rule *rules)
{
	int	ret;

	pthread_mutex_lock(&rules->state_lock);
	ret = rules->dead;
	pthread_mutex_unlock(&rules->state_lock);
	return (ret);
}

void	ft_set_dead(t_rule *rules)
{
	pthread_mutex_lock(&rules->state_lock);
	rules->dead = 1;
	pthread_mutex_unlock(&rules->state_lock);
}

void	ft_print_death(t_rule *rules, long id)
{
	pthread_mutex_lock(&rules->print_lock);
	printf("%ld %ld died\n", ft_get_time() - rules->time_start, id + 1);
	pthread_mutex_unlock(&rules->print_lock);
}

int	ft_check_starvation(t_rule *rules)
{
	int		i;
	long	last;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_lock(&rules->state_lock);
		last = rules->philos[i].last_meal;
		if (ft_get_time() - last > rules->time_to_die)
		{
			rules->dead = 1;
			pthread_mutex_unlock(&rules->state_lock);
			ft_print_death(rules, i);
			return (1);
		}
		pthread_mutex_unlock(&rules->state_lock);
		i++;
	}
	return (0);
}

int	ft_check_full(t_rule *rules)
{
	int	i;
	int	full;

	if (rules->number_of_meals < 0)
		return (0);
	i = 0;
	full = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_lock(&rules->state_lock);
		if (rules->philos[i].how_much_eat >= rules->number_of_meals)
			full++;
		pthread_mutex_unlock(&rules->state_lock);
		i++;
	}
	if (full < rules->number_of_philosophers)
		return (0);
	ft_set_dead(rules);
	return (1);
}
