NAME = philosophers
CCFLAGS = cc -Wall -Wextra -Werror -g3 philo/main.c -o philosophers -lpthread

all: $(NAME)

$(NAME):
	$(CCFLAGS)

clean:
	rm -rf philosophers

fclean: clean

re: fclean all
