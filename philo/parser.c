/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 02:34:22 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/09 03:15:46 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	ft_atol(char *str)
{
	int		i;
	int		sign;
	long	result;

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

int	validade_times(t_rule *rules)
{
	if (rules->time_to_die <= 0 || rules->time_to_eat <= 0
		|| rules->time_to_sleep <= 0)
		return (0);
	else if (rules->number_of_philosophers < 1
		|| rules->number_of_philosophers > 200)
		return (0);
	return (1);
}

int	check_arg(char *s)
{
	int	i;

	i = 0;
	if (ft_strlen(s) >= 11)
		return (1);
	if (s[i] == '+')
		i++;
	if (!s[i])
	    return (1);
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
		    return (1);
		i++;
	}
	return (0);
}

int	parse_arguments(int ac, char **args)
{
	int	i;

	i = 1;
	while (i < ac)
	{
		if (args[i][0] == 0 || check_arg(args[i]))
			return (1);
		i++;
	}
	return (0);
}
