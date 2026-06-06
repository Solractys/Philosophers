NAME		= philosophers
CCFLAGS		= cc -Wall -Wextra -Werror -g3

SRC_DIR		= philo
OBJ_DIR		= obj

SRC_FILES	= main.c \
			  init_philo.c \
			  utils/ft_atoi.c \
			  utils/print_error.c

SRCS		= $(addprefix $(SRC_DIR)/, $(SRC_FILES))
OBJS		= $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

GREEN		= \033[0;32m
RESET		= \033[0;31m

BANNER =	XXX     XX                                                     XXX   \
			XX      XX          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX            XX \
			X      XX       XXXXX                                XXXXX         XX\
			X     XX     XXXX        XXXXXXXXXXXXXXXXXXXXXXX         XXX        X\
			X     X     XX        XXXX                      XXX        XXX      X\
			X     X     X       XX                            XX         X       \
			X    XX    XX     XXX     ╔════════════╗           XX         X      \
			X    X     X      X       ║PHILOSOPHERS║            X         X      \
			X    X     X      X       ╚════════════╝           XX         X      \
			XX   X    X       X                 XX            XX          X      \
			 X   X    XX      XX            XXXX             XX          XX     X\
			 XX  X     XX      XXXXXXXXXXXXXX              XXX           X      X\
			  XX X      XXXXX                           XXXX           XX      XX\
			   XXX           XXX XXXXXXX        XXXXXXXXX            XXX      XX \
				XXX                    XXXXXXXXXX                  XXX       XX  \
				 XXX                                            XXXX       XXX   \

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@$(CCFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(CCFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)$(BANNER)$<$(RESET)"

unit-tests:
	@$(MAKE) -C ./philo/tests

clean:
	rm -rf philosophers
	rm -rf $(OBJ_DIR)
	@$(MAKE) clean -C ./philo/tests

fclean: clean
	@$(MAKE) fclean -C ./philo/tests

re: fclean all

.PHONY: all clean fclean re
