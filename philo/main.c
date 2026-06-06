/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/05 23:57:52 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/06 00:07:48 by csilva-s         ###   ########.fr       */
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
	free_rules(rules);
	return (0);
}
