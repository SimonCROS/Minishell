#include "minishell.h"

int	redirect_out(t_command *cmd)
{
	int			out_fd;
	t_entry		*walk;
	t_redirect	*redirect;

	if (lst_is_empty(cmd->redirect_out))
		return (TRUE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
		redirect = ((t_redirect *)walk->value);
		if (redirect->append)
			out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
		else
			out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			ft_puterr3(redirect->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(out_fd);
		else
			dup2(out_fd, redirect->fd);
		walk = walk->next;
	}
	close(out_fd);
	return (TRUE);
}

int	redirect_in(t_command *cmd)
{
	int			in_fd;
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_in))
		return (TRUE);
	walk = cmd->redirect_in->first;
	while (walk)
	{
		in_fd = open(((t_redirect *)walk->value)->str, O_RDWR);
		if (in_fd == -1)
		{
			ft_puterr3(((t_redirect *)walk->value)->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(in_fd);
		walk = walk->next;
	}
	dup2(in_fd, 0);
	close(in_fd);
	return (TRUE);
}

#include <stdio.h>
#include <sys/wait.h>

void	free_str_array(void **str_array)
{
	int	i;

	if (!str_array)
		return ;
	i = 0;
	while (str_array[i])
		free(str_array[i++]);
	free(str_array);
}

char	*get_path_from_env(char *path)
{
	t_list	*paths;
	char	*new_path;
	char	*result;
	char	*var;

	var = map_get(g_global.env, "PATH");
	if (!var)
	{
		errno = 2;
		ft_puterr3(path, ": ", strerror(errno));
		g_global.cmd_ret = errno;
		return (NULL);
	}
	new_path = ft_strjoin("/", path);
	paths = as_listf(ft_split(var, ':'), free);
	lst_map_in(paths, (t_map_opts){{ft_strjoin}, new_path, 1}, free);
	result = ft_strdup(lst_find_first(paths, file_exists));
	free(new_path);
	lst_destroy(paths);
	return (result);
}

pid_t	spawn_proc(int in, int out, t_command *cmd)
{
	int		status;
	pid_t	pid;
	char	*path;
	char	**argv;

	argv = (char **)as_array(cmd->args);
	path = argv[0];
	if (ft_strindex_of(path, '/') == -1)
		path = get_path_from_env(path);
	if (!path)
		return (-1); // free
	pid = fork();
	if (pid == 0)
	{
		// if (in != 0)
		// {
		// 	dup2(in, 0);
		// 	close(in);
		// }
		// if (out != 1)
		// {
		// 	dup2(out, 1);
		// 	close(out);
		// }
		parse(cmd);
		if (redirect_in(cmd) && redirect_out(cmd))
		{
			built_in(argv);
			if (!file_exists(path))
				exit(127);
			do_execute(path, argv);
			exit(EXIT_SUCCESS);
		}
		exit(EXIT_FAILURE);
	}
	// free(argv);
	waitpid(pid, &status, 0);
	g_global.cmd_ret = WEXITSTATUS(status);
	return (pid);
}

int	prepare_command(t_command *command, int in)
{
	int		fd[2];
	pid_t	pid;

	// pipe(fd);
	pid = spawn_proc(in, 0, command);
	// if (command->next_relation != T_PIPE)
	// 	waitpid(pid, NULL, 0);
	// close(fd[1]);
	// in = fd[0];
	return (0);
}

void	piper(t_command *command, t_iterator *it)
{
	int	in;

	in = prepare_command(command, 0);
	// while (command->next_relation == T_PIPE)
	// {
		// command = iterator_next(it);
		// in = prepare_command(command, in);
	// }
}

void	do_command(t_list *cmds)
{
	t_iterator	it;
	t_command	*cmd;

	g_global.cmd_ret = 0;
	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		cmd = iterator_next(&it);
		if (!parse(cmd))
		{
			g_global.cmd_ret = 2;
			continue ;
		}
		piper(cmd, &it);
	}
}
