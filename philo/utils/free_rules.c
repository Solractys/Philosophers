#include "../philo.h"

void	destroy_forks(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		if (rules->philos[i].left_fork)
		{
			pthread_mutex_destroy(&rules->philos[i].left_fork->mutex);
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
