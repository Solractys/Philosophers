/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_rules.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 02:58:33 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/11 02:58:34 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

void	destroy_forks(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		if (rules->philos[i].left_fork != NULL)
		{
			pthread_mutex_destroy(rules->philos[i].left_fork);
			free(rules->philos[i].left_fork);
		}
		i++;
	}
}

void	free_philosophers(t_rule *rules)
{
	if (!rules)
		return ;
	if (rules->philos)
	{
		destroy_forks(rules);
		free(rules->philos);
	}
}

void	free_rules(t_rule *rules)
{
	if (!rules)
		return ;
	if (rules->philos)
		free_philosophers(rules);
	free(rules);
}
