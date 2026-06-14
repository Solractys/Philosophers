/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test-parser.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/14 16:40:00 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/14 16:40:00 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tests.h"
#include "../philo.h"

/* -------------------------------- ft_atol ------------------------------- */

int	atol_should_parse_simple_positive(void)
{
	ASSERT_EQ(ft_atol("42"), 42);
	return (EXIT_SUCCESS);
}

int	atol_should_parse_zero(void)
{
	ASSERT_EQ(ft_atol("0"), 0);
	return (EXIT_SUCCESS);
}

int	atol_should_handle_leading_plus(void)
{
	ASSERT_EQ(ft_atol("+200"), 200);
	return (EXIT_SUCCESS);
}

int	atol_should_handle_negative(void)
{
	ASSERT_EQ(ft_atol("-5"), -5);
	return (EXIT_SUCCESS);
}

int	atol_should_skip_leading_whitespace(void)
{
	ASSERT_EQ(ft_atol("   \t7"), 7);
	return (EXIT_SUCCESS);
}

/* ------------- check_arg: returns 0 when valid, 1 when invalid ----------- */

int	check_arg_should_accept_digits(void)
{
	ASSERT_EQ(check_arg("123"), 0);
	return (EXIT_SUCCESS);
}

int	check_arg_should_accept_leading_plus(void)
{
	ASSERT_EQ(check_arg("+5"), 0);
	return (EXIT_SUCCESS);
}

int	check_arg_should_reject_negative_sign(void)
{
	ASSERT_EQ(check_arg("-5"), 1);
	return (EXIT_SUCCESS);
}

int	check_arg_should_reject_letters(void)
{
	ASSERT_EQ(check_arg("12a"), 1);
	return (EXIT_SUCCESS);
}

int	check_arg_should_reject_lone_plus(void)
{
	ASSERT_EQ(check_arg("+"), 1);
	return (EXIT_SUCCESS);
}

int	check_arg_should_reject_overlong_number(void)
{
	ASSERT_EQ(check_arg("12345678901"), 1);
	return (EXIT_SUCCESS);
}

/* ----- parse_arguments: 0 all valid, 1 any invalid (argv[0] ignored) ----- */

int	parse_should_accept_valid_set(void)
{
	char	*av[5] = {"philo", "5", "800", "200", "200"};

	ASSERT_EQ(parse_arguments(5, av), 0);
	return (EXIT_SUCCESS);
}

int	parse_should_reject_when_one_is_invalid(void)
{
	char	*av[5] = {"philo", "5", "800", "-200", "200"};

	ASSERT_EQ(parse_arguments(5, av), 1);
	return (EXIT_SUCCESS);
}

int	parse_should_reject_empty_argument(void)
{
	char	*av[5] = {"philo", "5", "", "200", "200"};

	ASSERT_EQ(parse_arguments(5, av), 1);
	return (EXIT_SUCCESS);
}

/* ----------- validade_times: returns 1 when valid, 0 when invalid -------- */

static void	set_times(t_rule *r, long n, long die, long eat, long sleep)
{
	r->number_of_philosophers = n;
	r->time_to_die = die;
	r->time_to_eat = eat;
	r->time_to_sleep = sleep;
}

int	validate_should_accept_sane_values(void)
{
	t_rule	r;

	set_times(&r, 5, 800, 200, 200);
	ASSERT_EQ(validade_times(&r), 1);
	return (EXIT_SUCCESS);
}

int	validate_should_reject_zero_philosophers(void)
{
	t_rule	r;

	set_times(&r, 0, 800, 200, 200);
	ASSERT_EQ(validade_times(&r), 0);
	return (EXIT_SUCCESS);
}

int	validate_should_reject_more_than_200(void)
{
	t_rule	r;

	set_times(&r, 201, 800, 200, 200);
	ASSERT_EQ(validade_times(&r), 0);
	return (EXIT_SUCCESS);
}

int	validate_should_reject_zero_time_to_die(void)
{
	t_rule	r;

	set_times(&r, 5, 0, 200, 200);
	ASSERT_EQ(validade_times(&r), 0);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(atol_should_parse_simple_positive);
	RUN_TEST(atol_should_parse_zero);
	RUN_TEST(atol_should_handle_leading_plus);
	RUN_TEST(atol_should_handle_negative);
	RUN_TEST(atol_should_skip_leading_whitespace);
	RUN_TEST(check_arg_should_accept_digits);
	RUN_TEST(check_arg_should_accept_leading_plus);
	RUN_TEST(check_arg_should_reject_negative_sign);
	RUN_TEST(check_arg_should_reject_letters);
	RUN_TEST(check_arg_should_reject_lone_plus);
	RUN_TEST(check_arg_should_reject_overlong_number);
	RUN_TEST(parse_should_accept_valid_set);
	RUN_TEST(parse_should_reject_when_one_is_invalid);
	RUN_TEST(parse_should_reject_empty_argument);
	RUN_TEST(validate_should_accept_sane_values);
	RUN_TEST(validate_should_reject_zero_philosophers);
	RUN_TEST(validate_should_reject_more_than_200);
	RUN_TEST(validate_should_reject_zero_time_to_die);
	return (0);
}
