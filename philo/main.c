/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 02:58:14 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/11 02:58:16 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	main(int ac, char **av)
{
	t_rule	*rules;

	if (ac < 5 || ac > 6)
	{
		print_error("Error: Invalid number of arguments\n");
		return (1);
	}
	rules = init_rules(ac, av);
	if (!rules)
	{
		print_error("Error: Invalid arguments\n");
		return (1);
	}
	// Start simluation

	free_rules(rules);

	return (0);
}
