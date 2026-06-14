/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 15:46:34 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/13 15:46:40 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ft_get_time(void)
{
	struct timeval	clock;

	gettimeofday(&clock, NULL);
	return ((clock.tv_sec * 1000) + (clock.tv_usec / 1000));
}

void	*ft_lonely(t_rule *rules, long id)
{
	ft_print_mutex("has taken a fork", id, rules);
	while (!ft_is_dead(rules))
		usleep(200);
	return (NULL);
}

void	*ft_routine(void *philo)
{
	t_philo	*tmp;

	tmp = (t_philo *)philo;
	if (tmp->rules->number_of_philosophers == 1)
		return (ft_lonely(tmp->rules, tmp->id));
	if (tmp->id % 2 == 0)
		usleep(1000);
	while (!ft_is_dead(tmp->rules))
	{
		ft_eat(tmp->rules, tmp->id);
		ft_print_mutex("is sleeping", tmp->id, tmp->rules);
		ft_usleep(tmp->rules, tmp->rules->time_to_sleep);
		ft_print_mutex("is thinking", tmp->id, tmp->rules);
	}
	return (NULL);
}

void	ft_join_threads(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_join(rules->philos[i].thread, NULL);
		i++;
	}
}

void	ft_init_simulation(t_rule *rules)
{
	int	i;

	i = 0;
	rules->time_start = ft_get_time();
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].last_meal = rules->time_start;
		pthread_create(&rules->philos[i].thread, NULL, ft_routine,
			&rules->philos[i]);
		i++;
	}
	ft_monitor(rules);
	ft_join_threads(rules);
}
