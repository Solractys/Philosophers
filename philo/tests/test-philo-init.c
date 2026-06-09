/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test-philo-init.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 01:46:45 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/09 01:52:10 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tests.h"
#include "../philo.h"

int	should_init_valid_rule_struct(void)
{
	char	*configs[5] = {"filename", "3", "800", "300", "300"};
	t_rule	*rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_EQ(rules->number_of_philosophers, 3);
	ASSERT_EQ(rules->time_to_die, 800);
	ASSERT_EQ(rules->time_to_eat, 300);
	ASSERT_EQ(rules->time_to_sleep, 300);
	ASSERT_NOT_NULL(rules->philos);
	int i = 0;
	while (i < rules->number_of_philosophers)
	{
		ASSERT_NOT_NULL(rules->philos[i].left_fork);
		i++;
	}
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_create_rule_struct_with_optional_meals(void)
{
	char *configs[6] = {"filename", "4", "800", "300", "300", "5"};
	t_rule *rules;
	rules = init_rules(6, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_EQ(rules->number_of_philosophers, 4);
	ASSERT_EQ(rules->time_to_die, 800);
	ASSERT_EQ(rules->time_to_eat, 300);
	ASSERT_EQ(rules->time_to_sleep, 300);
	ASSERT_EQ(rules->number_of_meals, 5);
	ASSERT_NOT_NULL(rules->philos);
	int i = 0;
	while (i < rules->number_of_philosophers)
	{
		ASSERT_NOT_NULL(rules->philos[i].left_fork);
		i++;
	}
	ASSERT_EQ(i, rules->number_of_philosophers);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_initialize_with_default_meals(void)
{
	char *configs[5] = {"filename", "4", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_EQ(rules->number_of_meals, -1);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_fail_with_invalid_number_of_philosophers(void)
{
	char *configs[5] = {"filename", "0", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NULL(rules);
	return (EXIT_SUCCESS);
}

int	should_fail_with_too_many_philosophers(void)
{
	char *configs[5] = {"filename", "201", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NULL(rules);
	return (EXIT_SUCCESS);
}

int	should_init_with_limit_philosophers(void)
{
	char *configs[5] = {"filename", "200", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	return (EXIT_SUCCESS);
}

int	should_init_with_two_philosophers(void)
{
	char *configs[5] = {"filename", "2", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	return (EXIT_SUCCESS);
}

int	should_init_with_one_philosophers(void)
{
	char *configs[5] = {"filename", "1", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_init_with_max_philosophers(void)
{
	char *configs[5] = {"filename", "199", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_fail_with_negative_time(void)
{
	char *configs[5] = {"filename", "5", "-800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NULL(rules);
	return (EXIT_SUCCESS);
}

int	should_init_forks_and_philosophers(void)
{
	char *configs[5] = {"filename", "5", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_NOT_NULL(rules->philos);
	int i = 0;
	while (i < rules->number_of_philosophers)
	{
		ASSERT_NOT_NULL(rules->philos[i].left_fork);
		i++;
	}
	ASSERT_EQ(i, rules->number_of_philosophers);
	free_rules(rules);

	return (EXIT_SUCCESS);
}

int	should_guard_free_rules_on_null(void)
{
	free_rules(NULL);
	return (EXIT_SUCCESS);
}

int	should_fail_with_invalid_strings_combinations(void)
{
	char *inputs[5] = {"filename", "5", "++&%420", "300", "300"};
	t_rule *rules = init_rules(5, inputs);
	ASSERT_NULL(rules);
	return (EXIT_SUCCESS);
}

int should_fail_with_invalid_integer(void)
{
    char *inputs[5] = {"filename", "+5", "8+00", "300", "300"};
    t_rule *rules = init_rules(5, inputs);
    ASSERT_NULL(rules);
    return (EXIT_SUCCESS);
}

int	should_create_left_and_right_forks(void)
{
	char *configs[5] = {"filename", "5", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	int i = 0;
	while (i < rules->number_of_philosophers)
	{
		ASSERT_NOT_NULL(rules->philos[i].left_fork);
		ASSERT_NOT_NULL(rules->philos[i].right_fork);
		i++;
	}
	ASSERT_EQ(i, rules->number_of_philosophers);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	should_create_left_and_right_forks_with_one_philo(void)
{
	char *configs[5] = {"filename", "1", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_NOT_NULL(rules->philos[0].left_fork);
	ASSERT_NOT_NULL(rules->philos[0].right_fork);
	free_rules(rules);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(should_init_valid_rule_struct);
	RUN_TEST(should_create_rule_struct_with_optional_meals);
	RUN_TEST(should_fail_with_invalid_number_of_philosophers);
	RUN_TEST(should_fail_with_invalid_strings_combinations);
	RUN_TEST(should_fail_with_too_many_philosophers);
	RUN_TEST(should_init_with_one_philosophers);
	RUN_TEST(should_init_with_two_philosophers);
	RUN_TEST(should_init_with_max_philosophers);
	RUN_TEST(should_fail_with_negative_time);
	RUN_TEST(should_init_forks_and_philosophers);
	RUN_TEST(should_initialize_with_default_meals);
	RUN_TEST(should_guard_free_rules_on_null);
	RUN_TEST(should_fail_with_invalid_integer);
	RUN_TEST(should_create_left_and_right_forks);
	RUN_TEST(should_create_left_and_right_forks_with_one_philo);
	return (0);
}
