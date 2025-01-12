/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 19:40:35 by aryamamo          #+#    #+#             */
/*   Updated: 2025/01/12 12:57:42 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../libft/libft.h"
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>

void	execve_error(char *cmd, int err_type);
void	invalid_args(void);
void	error(char *msg);
void	free_paths(char **paths);
char	*find_path(char *cmd, char **env);
void	execute(char *av, char **env);
void	child_process1(int infile, int *pipefd, char *cmd, char **env);
void	child_process2(int outfile, int *pipefd, char *cmd, char **env);
void	parent_process(int infile, int outfile, int *pipefd);
int		open_file(char *av, int n);

#endif
