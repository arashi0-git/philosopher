/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 10:11:01 by aryamamo          #+#    #+#             */
/*   Updated: 2025/01/12 13:18:01 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	child_process1(int infile, int *pipefd, char *cmd, char **env)
{
	if (dup2(infile, STDIN_FILENO) == -1)
		error("dup2 error");
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		error("dup2 error");
	close(pipefd[0]);
	close(pipefd[1]);
	close(infile);
	execute(cmd, env);
}

void	child_process2(int outfile, int *pipefd, char *cmd, char **env)
{
	if (dup2(pipefd[0], STDIN_FILENO) == -1)
		error("dup2 error");
	if (dup2(outfile, STDOUT_FILENO) == -1)
		error("dup2 error");
	close(pipefd[0]);
	close(pipefd[1]);
	close(outfile);
	execute(cmd, env);
}

void	parent_process(int infile, int outfile, int *pipefd)
{
	close(pipefd[0]);
	close(pipefd[1]);
	close(infile);
	close(outfile);
	wait(NULL);
	wait(NULL);
}

int	open_file(char *av, int n)
{
	int	fd;

	if (n == 0)
		fd = open(av, O_RDONLY);
	else
		fd = open(av, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("pipex: ", 2);
		ft_putstr_fd(av, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
		if (n != 0)
			exit(EXIT_FAILURE);
	}
	return (fd);
}

int	main(int ac, char **av, char **env)
{
	int		infile;
	int		outfile;
	int		pipefd[2];
	pid_t	pid1;
	pid_t	pid2;

	if (ac != 5)
		invalid_args();
	infile = open_file(av[1], 0);
	outfile = open_file(av[4], 1);
	if (pipe(pipefd) == -1)
		error("error create pipe");
	pid1 = fork();
	if (pid1 == -1)
		error("fork failed");
	if (pid1 == 0)
		child_process1(infile, pipefd, av[2], env);
	pid2 = fork();
	if (pid2 == -1)
		error("fork failed");
	if (pid2 == 0)
		child_process2(outfile, pipefd, av[3], env);
	parent_process(infile, outfile, pipefd);
	return (EXIT_SUCCESS);
}
