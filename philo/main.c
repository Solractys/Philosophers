#include "philo.h"

void print_struct(philo_t philo)
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

int	check_args_int(int ac, char **av)
{
	int	i;
	int	j;

	i = 1;
	while(i < ac)
	{
		j = 0;
		while (av[i][j])
		{
			if (av[i][j] < '0' || av[i][j] > '9')
			{
				print_error("Error: Non-integer argument detected\n");
				return (1);
			}
			j++;
		}
		i++;
	}
	return (0);
}

int	ft_atoi(char *str)
{
	int i;
	int	num;

	i = 0;
	num = 0;
	while (str[i] >=  '0' && str[i] <= '9')
	{
		num = num * 10 + (str[i++] - '0');
	}
	return (num);
}

philo_t	init_philo(int ac, char **av)
{
	philo_t	philo;

	philo.number_of_philosophers = ft_atoi(av[1]);
	if (philo.number_of_philosophers <= 200)
	{
		print_error("Warning: Number of philosophers is less than or equal to 200\n");
		return (philo);
	}
	philo.time_to_die = ft_atoi(av[2]);
	philo.time_to_eat = ft_atoi(av[3]);
	philo.time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		philo.number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	return (philo);
}

void	init_mutexes(philo_t *philo)
{

}

int	main(int ac, char **av)
{
	philo_t philo;

	if (ac < 5 || ac > 6)
	{
		print_error("Error: Invalid number of arguments\n");
		return (1);
	}
	if (check_args_int(ac, av))
		return (1);
	philo = init_philo(ac, av);
	print_struct(philo);
	return (0);
}
