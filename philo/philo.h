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
	long		number_of_philosophers;
	long		pos;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	long		number_of_meals;
	long		time_start;
	t_philo		*philos;
}	t_rule;

t_rule	*init_rules(int ac, char **av);
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
void	ft_usleep(t_rule *rules, long id, long time);
int		ft_sleeping(t_rule *rules, long id);
int		ft_dead(t_rule *rules, long id);
void	ft_print_mutex(char *str, long id, t_rule *rules);
int		ft_get_fork(t_rule *rules, long id);
int		ft_eating(t_rule *rules, long id);
void	ft_eat(t_rule *rules, long id);
#endif
