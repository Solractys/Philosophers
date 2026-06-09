# ==========================================================
#  config.mk — project-specific test configuration
#
#  This is the ONLY file you edit when dropping the tests/
#  folder into a new project. See examples/ for ready-made
#  configs (Philosophers, minishell).
# ==========================================================

# --- Project sources under test -------------------------------------
# Every .c file your tests call into. None of them may define main()!
# Paths are relative to this tests/ folder.
PROJECT_SRCS = ../parser.c \
							 ../utils/print_error.c \
							 ../utils/free_rules.c \
							 ../utils/utils.c \
							 ../init_philo.c


# --- Include paths ---------------------------------------------------
# -I flags so the compiler finds your project headers.
PROJECT_INCLUDES = -I..

# --- Link flags / libraries ------------------------------------------
# e.g. -pthread, -lm, -lreadline, ../lib/libft/libft.a
PROJECT_LIBS = -pthread

# --- Dependency libraries with their own Makefile (optional) ---------
# Each directory listed here gets `make` run inside it before the
# tests compile (e.g. ../lib/libft). Leave empty if not needed.
DEP_LIB_DIRS =

# --- Compiler overrides (optional) ------------------------------------
# CC     = cc
# CFLAGS = -g3 -Wall -Wextra -Werror
