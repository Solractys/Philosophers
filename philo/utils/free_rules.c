#include "../philo.h"

void	destroy_forks(t_rule *rules)
{
	int	i;

	i = 0;
	if (!rules->philos[0].left_fork)
		return ;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_destroy(&rules->philos[i].left_fork->mutex);
		free(rules->philos[i].left_fork);
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
