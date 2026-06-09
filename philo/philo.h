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

typedef struct s_fork
{
	pthread_mutex_t	mutex;
	int				id;
}	t_fork;

typedef struct s_rule	t_rule;

typedef struct s_philo
{
	int			id;
	pthread_t	thread;
	long		last_meal;
	t_fork		*left_fork;
	t_fork		*right_fork;
	t_rule		*rules;
}	t_philo;

typedef struct s_rule
{
	long		number_of_philosophers;
	long		time_to_die;
	long		time_to_eat;
	long		time_to_sleep;
	long		number_of_meals;
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

#endif
