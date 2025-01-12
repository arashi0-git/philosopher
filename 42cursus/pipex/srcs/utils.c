/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/12 12:44:05 by aryamamo          #+#    #+#             */
/*   Updated: 2025/01/12 14:29:06 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/pipex.h"

void	free_paths(char **paths)
{
	int	i;

	i = 0;
	while (paths[i])
		free(paths[i++]);
	free(paths);
}

char	*find_path(char *cmd, char **env)
{
	char	**paths;
	char	*path;
	int		i;
	char	*sep_path;

	i = 0;
	while (ft_strnstr(env[i], "PATH", 4) == 0)
		i++;
	paths = ft_split(env[i] + 5, ':');
	i = 0;
	while (paths[i])
	{
		sep_path = ft_strjoin(paths[i], "/");
		path = ft_strjoin(sep_path, cmd);
		free(sep_path);
		if (access(path, F_OK) == 0)
		{
			free_paths(paths);
			return (path);
		}
		free(path);
		i++;
	}
	free_paths(paths);
	return (NULL);
}

void	execute(char *av, char **env)
{
	char	**cmd;
	int		i;
	char	*path;

	i = 0;
	cmd = ft_split(av, ' ');
	path = find_path(cmd[0], env);
	if (!path)
	{
		execve_error(cmd[0], 0);
		while (cmd[i])
			free(cmd[i++]);
		free(cmd);
	}
	if (execve(path, cmd, env) == -1)
	{
		execve_error(cmd[0], EXIT_FAILURE);
		while (cmd[i])
			free(cmd[i++]);
		free(cmd);
		free(path);
	}
}
