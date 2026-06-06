#include "philo.h"

void print_struct(t_philo philo)
{
	printf("Number of Philosophers: %d\n", philo.number_of_philosophers);
	printf("Time to Die: %d\n", philo.time_to_die);
	printf("Time to Eat: %d\n", philo.time_to_eat);
	printf("Time to Sleep: %d\n", philo.time_to_sleep);
	printf("Number of Times Each Philosopher Must Eat: %d\n", philo.number_of_times_each_philosopher_must_eat);
}

void	print_error(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(2, &str[i], 1);
		i++;
	}
}

int	ft_atoi(char *str)
{
	int	i;
	int	sign;
	int	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result * sign);
}

t_philo	*init_philo(int ac, char **av)
{
	t_philo	*philo;

	philo = malloc(sizeof(t_philo));
	if (!philo)
		return (NULL);
	philo->number_of_philosophers = ft_atoi(av[1]);
	if (philo->number_of_philosophers > 200 || philo->number_of_philosophers <= 1)
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

int	main(int ac, char **av)
{
	t_philo *philo;

	if (ac < 5 || ac > 6)
	{
		print_error("Error: Invalid number of arguments\n");
		return (1);
	}
	philo = init_philo(ac, av);
	if (!philo)
	{
		print_error("Error: Invalid arguments\n");
		return (1);
	}
	print_struct(*philo);
	return (0);
}
