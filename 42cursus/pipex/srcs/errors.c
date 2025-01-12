/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 12:48:07 by aryamamo          #+#    #+#             */
/*   Updated: 2025/01/12 14:29:14 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	error(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void	execve_error(char *cmd, int err_type)
{
	if (err_type == 0)
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": command not found\n", 2);
	}
	else
	{
		ft_putstr_fd("pipex: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
		ft_putstr_fd(strerror(errno), 2);
		ft_putstr_fd("\n", 2);
	}
	_exit(err_type);
}

void	invalid_args(void)
{
	ft_putstr_fd("Error: Bad arguments\n", 2);
	ft_putstr_fd("./pipex <file1> <cmd1> <cmd2> <file2>\n", 1);
	exit(EXIT_FAILURE);
}
