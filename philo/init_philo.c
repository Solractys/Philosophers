/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_philo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 02:59:49 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/11 02:59:50 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_left_forks(t_rule *rules)
{
	int		i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		rules->philos[i].left_fork = malloc(sizeof(t_fork));
		if (!rules->philos[i].left_fork)
			return (0);
		if (pthread_mutex_init(&rules->philos[i].left_fork->mutex, NULL) != 0)
		{
			free(rules->philos[i].left_fork);
			rules->philos[i].left_fork = NULL;
			return (0);
		}
		rules->philos[i].left_fork->id = i;
		i++;
	}
	return (1);
}

void	link_forks(t_rule *rules)
{
	int		i;
	long	n;

	i = 0;
	n = rules->number_of_philosophers;
	while (i < n)
	{
		rules->philos[i].right_fork = rules->philos[(i + 1) % n].left_fork;
		i++;
	}
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
		rules->philos[i].left_fork = NULL;
		rules->philos[i].right_fork = NULL;
		rules->philos[i].rules = rules;
		i++;
	}
	if (!init_left_forks(rules))
		return (0);
	link_forks(rules);
	return (1);
}

t_rule	*init_rules(int ac, char **av)
{
	t_rule	*rules;

	if (parse_arguments(ac, av))
		return (NULL);
	rules = malloc(sizeof(t_rule));
	if (!rules)
		return (NULL);
	rules->philos = NULL;
	rules->number_of_philosophers = ft_atol(av[1]);
	rules->time_to_die = ft_atol(av[2]);
	rules->time_to_eat = ft_atol(av[3]);
	rules->time_to_sleep = ft_atol(av[4]);
	rules->number_of_meals = -1;
	if (ac == 6)
		rules->number_of_meals = ft_atol(av[5]);
	if (!validade_times(rules) || !init_philosophers(rules))
	{
		free_rules(rules);
		return (NULL);
	}
	return (rules);
}
