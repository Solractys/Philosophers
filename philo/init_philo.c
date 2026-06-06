#include "philo.h"

t_philo	*init_philo(int ac, char **av)
{
	t_philo	*philo;

	philo = malloc(sizeof(t_philo));
	if (!philo)
		return (NULL);
	philo->number_of_philosophers = ft_atoi(av[1]);
	if (philo->number_of_philosophers >= 200 || philo->number_of_philosophers <= 1)
	{
		print_error("Warning: Number of philosophers is less than 0 or equal to 200\n");
		return (NULL);
	}
	philo->time_to_die = ft_atoi(av[2]);
	philo->time_to_eat = ft_atoi(av[3]);
	philo->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		philo->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	return (philo);
}
