#include "../tests.h"

FILE	*g_test_log = NULL;
char	g_assert_msg[1024];

/*
** Under `make test` the makefile sets SHELLYROX_REPORT=1 and the results
** are appended to test_report.log. Run standalone (./unit_test.out, gdb,
** valgrind...) the results print to stderr instead — visible on screen,
** and the report file is never polluted with out-of-order lines.
*/
void	init_test_log(void)
{
	if (g_test_log != NULL)
		return ;
	if (getenv("SHELLYROX_REPORT") != NULL)
	{
		g_test_log = fopen("test_report.log", "a");
		if (!g_test_log)
		{
			perror("Failed to open test_report.log, logging to stderr");
			g_test_log = stderr;
		}
	}
	else
		g_test_log = stderr;
}
