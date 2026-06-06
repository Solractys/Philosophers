#include "../philo.h"

void	destroy_forks(t_rule *rules)
{
	int	i;

	i = 0;
	while (i < rules->number_of_philosophers)
	{
		pthread_mutex_destroy(&rules->forks[i].mutex);
		i++;
	}
	free(rules->forks);
}

void	free_rules(t_rule *rules)
{
	if (!rules)
		return ;
	if (rules->forks)
		destroy_forks(rules);
	if (rules->philos)
		free(rules->philos);
	free(rules);
}
