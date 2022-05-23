#include "minishell.h"

static void	launch_command2(t_command *cmd)
{
	char	**argv;
	char	*path;

	if (lst_is_empty(cmd->args) && !parse(cmd))
		exit(2);
	argv = (char **)as_array(cmd->args);
	if (argv && redirect_in(cmd) && redirect_out(cmd))
	{
		built_in(argv, TRUE);
		path = get_path_from_env(argv[0]);
		if (!path)
			exit(127);
		do_execute(path, argv);
		ft_puterr3(path, ": ", strerror(errno));
	}
	exit(EXIT_FAILURE);
}

static int	spawn_proc(int in, int out, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (in != 0)
		{
			dup2(in, 0);
			close(in);
		}
		if (out != 1)
		{
			dup2(out, 1);
			close(out);
		}
		launch_command2(cmd);
		exit(0);
	}
	return (pid);
}

static int	fork_pipes2(t_command *cmd, int *in)
{
	int	fd[2];

	if (!cmd)
		return (FALSE);
	if (cmd->next_relation != T_PIPE)
	{
		if (*in != 0)
			dup2(*in, 0);
		spawn_proc(*in, 1, cmd);
		return (TRUE);
	}
	pipe(fd);
	spawn_proc(*in, fd[1], cmd);
	close(fd[1]);
	*in = fd[0];
	return (FALSE);
}

int	fork_pipes(t_iterator *it)
{
	int			in;
	int			tmp;
	int			status;

	in = 0;
	status = 0;
	while (iterator_has_next(it))
		if (fork_pipes2(iterator_next(it), &in))
			break ;
	while (wait(&tmp) != -1)
		status = tmp;
	return (status);
}

int	launch_built_in(t_iterator *it)
{
	t_command	*command;
	char		**argv;
	int			ret;

	command = NULL;
	if (it->current)
		command = it->current->next->value;
	else if (it->list->first)
		command = it->list->first->value;
	if (!command || command->next_relation == T_PIPE)
		return (FALSE);
	parse(command);
	argv = (char **)as_array(command->args);
	ret = TRUE;
	if (argv && redirect_in(command) && redirect_out(command))
		ret = built_in(argv, FALSE);
	dup2(g_global.fd[0], 0);
	dup2(g_global.fd[1], 1);
	dup2(g_global.fd[2], 2);
	free(argv);
	if (ret)
		iterator_next(it);
	return (ret);
}
