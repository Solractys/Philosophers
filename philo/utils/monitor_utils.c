/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 18:16:27 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/14 18:16:28 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

void	ft_monitor(t_rule *rules)
{
	while (1)
	{
		if (ft_check_starvation(rules))
			return ;
		if (ft_check_full(rules))
			return ;
		usleep(500);
	}
}
