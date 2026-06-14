/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: csilva-s <csilva-s@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 01:51:23 by csilva-s          #+#    #+#             */
/*   Updated: 2026/06/09 03:13:01 by csilva-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H
# include <pthread.h>
# include <sys/time.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <string.h>

typedef struct s_rule	t_rule;

typedef struct s_philo
{
	long				id;
	pthread_t			thread;
	long				last_meal;
	int					how_much_eat;
	int					is_dead;
	pthread_mutex_t		*left_fork;
	pthread_mutex_t		*right_fork;
	t_rule				*rules;
}	t_philo;

typedef struct s_rule
{
	long				number_of_philosophers;
	long				time_to_die;
	long				time_to_eat;
	long				time_to_sleep;
	long				number_of_meals;
	long				time_start;
	int					dead;
	pthread_mutex_t		print_lock;
	pthread_mutex_t		state_lock;
	t_philo				*philos;
}	t_rule;

t_rule	*init_rules(int ac, char **av);
void	put_number_on_table(t_rule **rules, int ac, char **av);
int		init_left_forks(t_rule *rules);
void	link_forks(t_rule *rules);
int		init_philosophers(t_rule *rules);
void	print_error(char *str);
void	free_rules(t_rule *rules);

// parser
int		check_arg(char *s);
int		parse_arguments(int ac, char **args);
int		validade_times(t_rule *rules);
long	ft_atol(char *str);
size_t	ft_strlen(char *s);

// Time functions
long	ft_get_time(void);

// Routine functions
void	ft_usleep(t_rule *rules, long time);
void	ft_print_mutex(char *str, long id, t_rule *rules);
int		ft_get_fork(t_rule *rules, long id);
void	ft_eat(t_rule *rules, long id);
int		ft_put_fork(t_rule *rules, long id);

// State and monitor functions
int		ft_is_dead(t_rule *rules);
void	ft_set_dead(t_rule *rules);
void	ft_print_death(t_rule *rules, long id);
void	ft_monitor(t_rule *rules);

// Simulations functions
void	ft_init_simulation(t_rule *rules);
void	ft_join_threads(t_rule *rules);
// Monitor functions
int		ft_check_starvation(t_rule *rules);
int		ft_check_full(t_rule *rules);
#endif
