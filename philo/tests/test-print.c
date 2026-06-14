/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test-print.c                                       :+:      :+:    :+:   */
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

/* ------------------------------ ft_print_mutex -------------------------- */

/* id is 0-based internally; the printed number must be id + 1.            */
int	print_mutex_should_emit_id_and_status(void)
{
	t_rule	*rules;
	int		saved;
	char	*out;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	saved = start_capture();
	ASSERT_TRUE(saved != -1);
	ft_print_mutex("is eating", 2, rules);
	out = end_capture(saved);
	free_rules(rules);
	ASSERT_NOT_NULL(out);
	ASSERT_TRUE(strstr(out, " 3 is eating") != NULL);
	ASSERT_TRUE(out[0] != '\0' && out[strlen(out) - 1] == '\n');
	free(out);
	return (EXIT_SUCCESS);
}

/* Nothing must be printed once the simulation is flagged as over.         */
int	print_mutex_should_be_silent_when_dead(void)
{
	t_rule	*rules;
	int		saved;
	char	*out;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	ft_set_dead(rules);
	saved = start_capture();
	ASSERT_TRUE(saved != -1);
	ft_print_mutex("is eating", 0, rules);
	out = end_capture(saved);
	free_rules(rules);
	ASSERT_NOT_NULL(out);
	ASSERT_STR_EQ(out, "");
	free(out);
	return (EXIT_SUCCESS);
}

/* ------------------------------ ft_print_death -------------------------- */

int	print_death_should_emit_died(void)
{
	t_rule	*rules;
	int		saved;
	char	*out;

	rules = fresh_rules();
	ASSERT_NOT_NULL(rules);
	saved = start_capture();
	ASSERT_TRUE(saved != -1);
	ft_print_death(rules, 0);
	out = end_capture(saved);
	free_rules(rules);
	ASSERT_NOT_NULL(out);
	ASSERT_TRUE(strstr(out, " 1 died") != NULL);
	free(out);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(print_mutex_should_emit_id_and_status);
	RUN_TEST(print_mutex_should_be_silent_when_dead);
	RUN_TEST(print_death_should_emit_died);
	return (0);
}
