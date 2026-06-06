/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_error.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 23:57:37 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/05 23:58:10 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

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
