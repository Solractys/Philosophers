# Shellyrox-Tester — automated tests framework

_fconde-p && csilva-s_

## Description

This test framework intends to help you keep your development process healthy and stable. When we develop code we tend to break things. And, as mature programmers, we know that the best way to do and maintain good work is both by documenting our project and having quick feedback when something goes wrong while growing our systems.

So here we have a helper. Think about the time spent with testing as a bigger amount of time saved from infernal helter-skelter debugging sessions. The tests you make should provide you a safety net. Think about their granularity, their modularity and the strategy you use with this approach.

Born inside a minishell, the framework is now **project-agnostic**: drop the `tests/` folder into any C project, fill in one config file, write your tests, run `make test`.

_Read more about different types of tests like unit, regression, functional, non-functional, black/white box and many others._

## Quick start

1. Copy this `tests/` folder into the root of your project.
2. Edit `config.mk` — the **only** file you ever edit (see `examples/` for ready-made configs for Philosophers and minishell).
3. Write test files named `test-<anything>.c` inside `tests/`. They are **auto-discovered** — no registration needed.
4. Run:

```sh
cd tests && make test
```

Want to see it working before plugging your project in? The example runs out of the box:

```sh
cp examples/test-example.c . && make test
```

## Main structure

| File | Responsibility |
|---|---|
| `makefile` | Framework logic: compiles each test file independently, runs it, writes the report, signals _success_/_failure_ at the very end. **Never edit it.** |
| `config.mk` | Project-specific settings: sources under test, include paths, link flags, dependency libs. **The only file you edit.** |
| `tests.h` | Framework header: the `RUN_TEST` runner, assert macros, stdout-capture helpers. |
| `test-{name}.c` | Your test files. Each contains a `main` that calls `RUN_TEST` on its cases. |
| `utils/` | Framework internals (stdout capture, report logging). |
| `examples/` | Sample test file + ready-made `config.mk` for Philosophers and minishell. |
| `test_report.log` | The generated report of the latest run. |

## config.mk reference

```makefile
# Every .c your tests call into — none of them may define main()!
PROJECT_SRCS     = ../src/time_utils.c ../src/routine.c

# -I flags so the compiler finds your headers
PROJECT_INCLUDES = -I.. -I../includes

# Link flags: -pthread, -lm, -lreadline, path/to/lib.a ...
PROJECT_LIBS     = -pthread

# Dirs with their own Makefile, built before the tests (e.g. ../lib/libft)
DEP_LIB_DIRS     =

# Optional overrides:
# CC     = cc
# CFLAGS = -g3 -Wall -Wextra -Werror
```

> **Important:** `PROJECT_SRCS` must not contain the file that defines your project's `main()` — each test file provides its own.

## Writing tests

A test is a function taking no arguments and returning `int`: `0` (`EXIT_SUCCESS`) means pass, anything else means fail. Register cases with `RUN_TEST` in `main`:

```c
#include "tests.h"
#include "philo.h" /* your project header */

int	time_must_move_forward(void)
{
	long	before;

	before = get_time_ms();
	usleep(50 * 1000);
	ASSERT_TRUE(get_time_ms() - before >= 40);
	return (EXIT_SUCCESS);
}

int	main(void)
{
	RUN_TEST(time_must_move_forward);
	return (0);
}
```

### Assert macros

On failure they record the reason — shown right under the `[FAIL]` line in the report, with file and line — and return `EXIT_FAILURE` from the test immediately.

| Macro | Checks |
|---|---|
| `ASSERT_TRUE(cond)` | `cond` is non-zero |
| `ASSERT_FALSE(cond)` | `cond` is zero |
| `ASSERT_EQ(actual, expected)` | integers are equal (reports expected vs. got) |
| `ASSERT_NEQ(actual, not_expected)` | integers differ |
| `ASSERT_STR_EQ(actual, expected)` | strings are equal (`NULL`-safe) |
| `ASSERT_NULL(ptr)` / `ASSERT_NOT_NULL(ptr)` | pointer is / is not `NULL` |
| `TEST_FAILF("msg %d", x)` | fails unconditionally with a custom printf-style message |

The plain style (`if (...) return (EXIT_FAILURE);`) still works exactly as before — asserts are optional sugar.

> Since asserts return early, free your resources *before* asserting — or accept the small leak; the test process exits right afterwards anyway.

### Capturing stdout

To test functions that print, wrap them with the capture helpers:

```c
int	greet_must_print_hello(void)
{
	int		saved;
	char	*out;

	saved = start_capture();
	ASSERT_TRUE(saved != -1);
	greet();
	out = end_capture(saved);
	ASSERT_NOT_NULL(out);
	ASSERT_STR_EQ(out, "hello, tester!\n");
	free(out);
	return (EXIT_SUCCESS);
}
```

## Main commands

**make test** → Run every `test-*.c` found in the folder. Each file is compiled and executed independently, so one broken file never blocks the others:

- compiles fine, all cases return 0 → `[PASS]` per case
- compiles fine, a case returns non-zero → `[FAIL]` (with assert details when available)
- doesn't compile → `[ERROR] Compilation failed` (compiler output included in the report)
- crashes at runtime (segfault etc.) → `[ERROR] Runtime failure (exit code N)` — note that the cases the crashed binary didn't reach are not counted
- no `test-*.c` found at all → `[!] No tests were run.` and a failing exit code, so a misconfigured CI never reports a green build by accident

The exit code is `0` only if everything passed — ready to trigger any CI/CD process. On failure, check for **non-zero** rather than a specific value: GNU make reports the failure as its own exit status `2` and prints a `make: *** [test] Error 1` line — that's expected behavior, not a broken makefile.

In the summary, **Total cases** counts executed cases (`Pass + Fail`); **Errors** counts files that failed to compile or crashed, whose cases never got to run.

**make single T={NAME}** → Compile `test-{NAME}.c` into a persistent `unit_test.out`, perfect for **gdb**/**valgrind** sessions. Run directly, the binary prints its results to **stderr** (only under `make test` do results go to the report file).

**make clean** → Remove stray `.out` binaries.

**make fclean** → `clean` + remove the report log. (Dependency libs are *not* cleaned — they belong to the parent project.)

## Report sample

```
TEST REPORT - Sat Jun  6 00:48:54 2026
------------------------------------------

File: ./test-math.c
      [PASS] Case: add_two_plus_two_must_be_four
      [FAIL] Case: add_must_handle_negatives
             add(-2, 2): expected 1, got 0 (test-math.c:14)

==========================================
📊 Summary:
  Total cases:  2
  ✅ Pass:      1
  ❌ Fail:      1
  ⚠️  Errors:    0
==========================================
```
