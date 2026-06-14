/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test-state.c                                       :+:      :+:    :+:   */
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
	char	*av[5] = {"philo", "3", "800", "200", "200"};

	return (init_rules(5, av));
}

/* ----------------------- ft_is_dead / ft_set_dead ----------------------- */

int	is_dead_should_be_false_after_init(void)
{
	t_rule	*rules;
	int		ret;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	ret = ft_is_dead(rules);
	free_rules(rules);
	ASSERT_EQ(ret, 0);
	return (EXIT_SUCCESS);
}

int	set_dead_should_flip_flag(void)
{
	t_rule	*rules;
	int		ret;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	ft_set_dead(rules);
	ret = ft_is_dead(rules);
	free_rules(rules);
	ASSERT_EQ(ret, 1);
	return (EXIT_SUCCESS);
}

int	set_dead_should_be_idempotent(void)
{
	t_rule	*rules;
	int		ret;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	ft_set_dead(rules);
	ft_set_dead(rules);
	ret = ft_is_dead(rules);
	free_rules(rules);
	ASSERT_EQ(ret, 1);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(is_dead_should_be_false_after_init);
	RUN_TEST(set_dead_should_flip_flag);
	RUN_TEST(set_dead_should_be_idempotent);
	return (0);
}
