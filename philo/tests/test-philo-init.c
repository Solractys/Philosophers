#include "tests.h"
#include "../philo.h"

int	should_init_valid_philo_struct(void)
{
	char *configs[5] = {"filename", "3", "800", "300", "300"};
	t_philo *philo;
	philo = init_philo(5, configs);
	ASSERT_NOT_NULL(philo);
	ASSERT_EQ(philo->number_of_philosophers, 3);
	ASSERT_EQ(philo->time_to_die, 800);
	ASSERT_EQ(philo->time_to_eat, 300);
	ASSERT_EQ(philo->time_to_sleep, 300);
	free(philo);
	return (EXIT_SUCCESS);
}

int	should_create_philo_struct_with_optional_meals(void)
{
	char *configs[6] = {"filename", "4", "800", "300", "300", "5"};
	t_philo *philo;
	philo = init_philo(6, configs);
	ASSERT_NOT_NULL(philo);
	ASSERT_EQ(philo->number_of_philosophers, 4);
	ASSERT_EQ(philo->time_to_die, 800);
	ASSERT_EQ(philo->time_to_eat, 300);
	ASSERT_EQ(philo->time_to_sleep, 300);
	ASSERT_EQ(philo->number_of_times_each_philosopher_must_eat, 5);
	free(philo);
	return (EXIT_SUCCESS);
}

int	should_fail_with_invalid_number_of_philosophers(void)
{
	char *configs[5] = {"filename", "0", "800", "300", "300"};
	t_philo *philo;
	philo = init_philo(5, configs);
	ASSERT_NULL(philo);
	return (EXIT_SUCCESS);
}

int	should_fail_with_too_many_philosophers(void)
{
	char *configs[5] = {"filename", "201", "800", "300", "300"};
	t_philo *philo;
	philo = init_philo(5, configs);
	ASSERT_NULL(philo);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(should_init_valid_philo_struct);
	RUN_TEST(should_create_philo_struct_with_optional_meals);
	RUN_TEST(should_fail_with_invalid_number_of_philosophers);
	RUN_TEST(should_fail_with_too_many_philosophers);
	return (0);
}
