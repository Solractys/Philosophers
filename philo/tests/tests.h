#ifndef TESTS_H
# define TESTS_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>

/* ************************************************************************** */
/* Framework resources                                                        */
/* ************************************************************************** */

int			start_capture(void);
char		*end_capture(int saved_stdout);
void		init_test_log(void);

extern FILE	*g_test_log;
extern char	g_assert_msg[1024];

/* ************************************************************************** */
/* Test runner                                                                */
/*                                                                            */
/* Each test function takes no arguments and returns an int:                  */
/*   0 (EXIT_SUCCESS) -> [PASS]                                               */
/*   non-zero         -> [FAIL]                                               */
/* ************************************************************************** */

# define RUN_TEST(test_func) \
	do { \
		init_test_log(); \
		g_assert_msg[0] = '\0'; \
		if (test_func() != 0) { \
			fprintf(g_test_log, "      [FAIL] Case: %s\n", #test_func); \
			if (g_assert_msg[0] != '\0') \
				fprintf(g_test_log, "             %s\n", g_assert_msg); \
		} else { \
			fprintf(g_test_log, "      [PASS] Case: %s\n", #test_func); \
		} \
		fflush(g_test_log); \
	} while (0)

/* ************************************************************************** */
/* Assertions                                                                 */
/*                                                                            */
/* On failure they record the reason (printed under the [FAIL] line of the    */
/* report) and make the test function return EXIT_FAILURE immediately.        */
/* NOTE: since they return early, free your resources *before* asserting --   */
/* or accept the small leak (the test process exits right after anyway).      */
/* ************************************************************************** */

/* Fail the test with a custom printf-style message. */
# define TEST_FAILF(...) \
	do { \
		snprintf(g_assert_msg, sizeof(g_assert_msg), __VA_ARGS__); \
		return (EXIT_FAILURE); \
	} while (0)

# define ASSERT_TRUE(cond) \
	do { \
		if (!(cond)) \
			TEST_FAILF("ASSERT_TRUE(%s) failed (%s:%d)", \
				#cond, __FILE__, __LINE__); \
	} while (0)

# define ASSERT_FALSE(cond) \
	do { \
		if (cond) \
			TEST_FAILF("ASSERT_FALSE(%s) failed (%s:%d)", \
				#cond, __FILE__, __LINE__); \
	} while (0)

/* Integer comparison (chars, sizes, enums... casts to long — unsigned    */
/* values above LONG_MAX will display as negative in the message).         */
# define ASSERT_EQ(actual, expected) \
	do { \
		long	_sx_actual = (long)(actual); \
		long	_sx_expected = (long)(expected); \
		if (_sx_actual != _sx_expected) \
			TEST_FAILF("%s: expected %ld, got %ld (%s:%d)", \
				#actual, _sx_expected, _sx_actual, __FILE__, __LINE__); \
	} while (0)

# define ASSERT_NEQ(actual, not_expected) \
	do { \
		long	_sx_actual = (long)(actual); \
		long	_sx_not_exp = (long)(not_expected); \
		if (_sx_actual == _sx_not_exp) \
			TEST_FAILF("%s: got %ld but expected anything else (%s:%d)", \
				#actual, _sx_actual, __FILE__, __LINE__); \
	} while (0)

/* String comparison. Two NULLs are considered equal. */
# define ASSERT_STR_EQ(actual, expected) \
	do { \
		const char	*_sx_actual = (actual); \
		const char	*_sx_expected = (expected); \
		if (((_sx_actual == NULL) != (_sx_expected == NULL)) \
			|| (_sx_actual && _sx_expected \
				&& strcmp(_sx_actual, _sx_expected) != 0)) \
			TEST_FAILF("%s: expected \"%s\", got \"%s\" (%s:%d)", \
				#actual, _sx_expected ? _sx_expected : "(null)", \
				_sx_actual ? _sx_actual : "(null)", \
				__FILE__, __LINE__); \
	} while (0)

# define ASSERT_NULL(ptr) \
	do { \
		if ((ptr) != NULL) \
			TEST_FAILF("%s: expected NULL (%s:%d)", \
				#ptr, __FILE__, __LINE__); \
	} while (0)

# define ASSERT_NOT_NULL(ptr) \
	do { \
		if ((ptr) == NULL) \
			TEST_FAILF("%s: unexpected NULL (%s:%d)", \
				#ptr, __FILE__, __LINE__); \
	} while (0)

#endif
