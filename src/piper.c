#include "minishell.h"

void	launch_command2(t_command *cmd)
{
	char	**argv;
	char	*path;

	if (!parse(cmd))
		exit(2);
	argv = (char **)as_array(cmd->args);
	if (argv && redirect_in(cmd) && redirect_out(cmd))
	{
		built_in(argv, TRUE);
		path = get_path_from_env(argv[0]);
		if (!path)
			exit(errno);
		do_execute(path, argv);
		ft_puterr3(path, ": ", strerror(errno));
	}
	exit(EXIT_FAILURE);
}

int	spawn_proc(int in, int out, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (pid);
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

int	fork_pipes2(t_command *cmd, int *count, int *in, int *last)
{
	int	fd[2];

	if (!cmd)
		return (FALSE);
	++*count;
	if (cmd->next_relation != T_PIPE)
	{
		if (*in != 0)
			dup2(*in, 0);
		*last = spawn_proc(*in, 1, cmd);
		if (*last == -1)
			--*count;
		return (TRUE);
	}
	pipe(fd);
	if (spawn_proc(*in, fd[1], cmd) == -1)
		--*count;
	close(fd[1]);
	*in = fd[0];
	return (FALSE);
}

int	fork_pipes(t_iterator *it)
{
	pid_t		last;
	int			count;
	int			in;
	int			tmp;
	int			status;

	count = 0;
	in = 0;
	status = 0;
	while (iterator_has_next(it))
		if (fork_pipes2(iterator_next(it), &count, &in, &last))
			break ;
	while (count--)
		if (wait(&tmp) == last)
			status = tmp;
	exit(WEXITSTATUS(status));
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
	free(argv);
	if (ret)
		iterator_next(it);
	else
		lst_clear(command->args);
	return (ret);
}

void	do_command(t_list *cmds)
{
	t_iterator	it;
	int			status;
	pid_t		pid;

	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		if (launch_built_in(&it))
			continue ;
		pid = fork();
		if (pid == -1)
			break ;
		if (!pid)
			fork_pipes(&it);
		pid = waitpid(pid, &status, 0);
		g_global.cmd_ret = WEXITSTATUS(status);
		while (((t_command *)iterator_next(&it))->next_relation == T_PIPE)
			;
	}
}
