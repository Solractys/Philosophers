#ifndef PHILO_H
# define PHILO_H
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>

typedef struct s_fork
{
	pthread_mutex_t	mutex;
	int				id;
}	t_fork;

typedef struct s_rule t_rule;

typedef struct s_philo
{
	int			id;
	pthread_t	thread;
	long		last_meal;
	t_fork		*left_fork;
	t_rule		*rules;
} t_philo;

typedef struct  s_rule
{
	int		number_of_philosophers;
	int		time_to_die;
	int		time_to_eat;
	int		time_to_sleep;
	int		number_of_meals;
	t_philo	*philos;
}	t_rule;

t_rule	*init_rules(int ac, char **av);
int		ft_atoi(char *str);
int		init_forks(t_rule *rules);
int		init_philosophers(t_rule *rules);
int		validade_times(t_rule *rules);
void	print_error(char *str);
void	free_rules(t_rule *rules);

#endif
