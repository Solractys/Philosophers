/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test-time.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:40:00 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/14 16:40:00 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tests.h"
#include "../philo.h"

static t_rule	*fresh_rules(void)
{
	char	*av[5] = {"philo", "2", "800", "200", "200"};

	return (init_rules(5, av));
}

/* ------------------------------ ft_get_time ----------------------------- */

int	get_time_should_be_positive(void)
{
	ASSERT_TRUE(ft_get_time() > 0);
	return (EXIT_SUCCESS);
}

int	get_time_should_move_forward(void)
{
	long	before;

	before = ft_get_time();
	usleep(30 * 1000);
	ASSERT_TRUE(ft_get_time() - before >= 25);
	return (EXIT_SUCCESS);
}

/* -------------------------------- ft_usleep ----------------------------- */

int	usleep_should_wait_requested_time(void)
{
	t_rule	*rules;
	long	before;
	long	elapsed;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	before = ft_get_time();
	ft_usleep(rules, 100);
	elapsed = ft_get_time() - before;
	free_rules(rules);
	ASSERT_TRUE(elapsed >= 90);
	ASSERT_TRUE(elapsed < 200);
	return (EXIT_SUCCESS);
}

int	usleep_should_return_early_when_dead(void)
{
	t_rule	*rules;
	long	before;
	long	elapsed;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	ft_set_dead(rules);
	before = ft_get_time();
	ft_usleep(rules, 5000);
	elapsed = ft_get_time() - before;
	free_rules(rules);
	ASSERT_TRUE(elapsed < 100);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(get_time_should_be_positive);
	RUN_TEST(get_time_should_move_forward);
	RUN_TEST(usleep_should_wait_requested_time);
	RUN_TEST(usleep_should_return_early_when_dead);
	return (0);
}
