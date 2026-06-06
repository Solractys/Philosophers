#include "tests.h"
#include "../philo.h"

int	should_init_valid_rule_struct(void)
{
	char *configs[5] = {"filename", "3", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NOT_NULL(rules);
	ASSERT_EQ(rules->number_of_philosophers, 3);
	ASSERT_EQ(rules->time_to_die, 800);
	ASSERT_EQ(rules->time_to_eat, 300);
	ASSERT_EQ(rules->time_to_sleep, 300);
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

int	should_fail_with_limit_philosophers(void)
{
	char *configs[5] = {"filename", "200", "800", "300", "300"};
	t_rule *rules;
	rules = init_rules(5, configs);
	ASSERT_NULL(rules);
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
	ASSERT_NOT_NULL(rules->forks);
	ASSERT_NOT_NULL(rules->philos);
	free_rules(rules);

	return (EXIT_SUCCESS);
}

int	should_guard_free_rules_on_null(void)
{
	free_rules(NULL);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(should_init_valid_rule_struct);
	RUN_TEST(should_create_rule_struct_with_optional_meals);
	RUN_TEST(should_fail_with_invalid_number_of_philosophers);
	RUN_TEST(should_fail_with_limit_philosophers);
	RUN_TEST(should_fail_with_too_many_philosophers);
	RUN_TEST(should_init_with_two_philosophers);
	RUN_TEST(should_init_with_max_philosophers);
	RUN_TEST(should_fail_with_negative_time);
	RUN_TEST(should_init_forks_and_philosophers);
	RUN_TEST(should_initialize_with_default_meals);
	RUN_TEST(should_guard_free_rules_on_null);
	return (0);
}
