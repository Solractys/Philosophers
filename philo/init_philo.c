/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/06 04:29:42 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/06 04:29:43 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_forks(t_rule *rules)
{
	int	i;

	i = 0;
	rules->forks = malloc(sizeof(t_fork) * rules->number_of_philosophers);
	if (!rules->forks)
		return (0);
	while (i < rules->number_of_philosophers)
	{
		rules->forks[i].id = i;
		pthread_mutex_init(&rules->forks[i].mutex, NULL);
		i++;
	}
	return (1);
}

int	init_philosophers(t_rule *rules)
{
	int	i;

	i = 0;
	rules->philos = malloc(sizeof(t_philo) * rules->number_of_philosophers);
	if (!rules->philos)
		return (0);
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].id = i + 1;
		rules->philos[i].last_meal = 0;
		rules->philos[i].rules = rules;
		i++;
	}
	return (1);
}

int	validade_times(t_rule *rules)
{
	if (rules->time_to_die < 0 || rules->time_to_eat < 0 ||
		rules->time_to_sleep < 0)
		return (0);
	else if (rules->number_of_philosophers <= 1 ||
		rules->number_of_philosophers >= 200)
		return (0);
	return (1);
}

t_rule	*init_rules(int ac, char **av)
{
	t_rule	*rules;

	rules = malloc(sizeof(t_rule));
	if (!rules)
		return (NULL);
	rules->forks = NULL;
	rules->philos = NULL;
	rules->number_of_philosophers = ft_atoi(av[1]);
	rules->time_to_die = ft_atoi(av[2]);
	rules->time_to_eat = ft_atoi(av[3]);
	rules->time_to_sleep = ft_atoi(av[4]);
	rules->number_of_meals = -1;
	if (ac == 6)
		rules->number_of_meals = ft_atoi(av[5]);
	if (!validade_times(rules) || !init_forks(rules) || !init_philosophers(rules))
	{
		free_rules(rules);
		return (NULL);
	}
	return (rules);
}
