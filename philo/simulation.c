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
#include <pthread.h>

// Iniciar a simulaćão
//		- Preciso de iniciar o tempo
// 			- Preciso de uma funćão que pega o tempo
// 			- Preciso de outra funcão que passa o tempo de bloco em bloco sem ser a
// 				usleep()
// 		- O filosofo precisa saber que horas são
// 		- O filosofo precisa saber o que ele vai fazer
// 		- O filosofo precisa pegar o garfo sem se atrapalhar com os vizinhos

long	ft_get_time(void)
{
	struct timeval	clock;

	gettimeofday(&clock, NULL);
	return ((clock.tv_sec * 1000) + (clock.tv_usec / 1000));
}

void	*ft_routine(void *rules)
{
	t_rule	*tmp;
	int		i;

	tmp = (t_rule *)rules;
	i = tmp->pos;
	while (!tmp->philos[i].is_dead)
	{
		ft_eating(rules, tmp->philos[i].id);
		ft_sleeping(rules, tmp->philos[i].id);
	}
}

void	ft_init_simulation(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_create(&rules->philos[i].thread, NULL, ft_routine, &rules);
		rules->pos++;
		i++;
	}
}
