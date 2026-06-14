NAME		= philo/philo
CCFLAGS		= cc -Wall -Wextra -Werror -g3

SRC_DIR		= philo
OBJ_DIR		= obj

SRC_FILES	= main.c \
						parser.c \
						init_philo.c \
						utils/utils.c \
						utils/print_error.c \
						simulations.c \
						utils/free_rules.c

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

GREEN		= \033[0;34m
RESET		= \033[0m

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CCFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CCFLAGS) $(OBJS) -o $(NAME)
	@printf "$(GREEN)"
	@printf "     _________    ___  ___     ___     ___        ______ \n"
	@printf "    /\   __   \  /\  \ \  \   /\  \   /\  \      /\  __ \ \n"
	@printf "    \ \  \_/\  \ \ \  \_\  \  \ \  \  \ \  \     \ \ \ \ \ \n"
	@printf "     \ \   _____\ \ \   __  \  \ \  \  \ \  \     \ \ \ \ \ \n"
	@printf "      \ \  \____/  \ \  \ \  \  \ \  \  \ \  \____ \ \ \_\ \ \n"
	@printf "       \ \__\       \ \__\ \__\  \ \__\  \ \______\ \ \_____\ \n"
	@printf "        \/__/        \/__/\/__/   \/__/   \/______/  \/_____/  \n"
	@printf "\t\t\t\t\t\t by csilva-s\n"
	@printf "$(RESET)"
unit-tests:
	@$(MAKE) -C ./philo/tests

clean:
	rm -rf philo/philo
	rm -rf $(OBJ_DIR)
	@$(MAKE) clean -C ./philo/tests

fclean: clean
	@$(MAKE) fclean -C ./philo/tests

re: fclean all

.PHONY: all clean fclean re
