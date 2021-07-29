#include "minishell.h"

void	do_command(t_list *cmds)
{
	t_iterator	it;
	int			status;

	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		if (launch_built_in(&it))
			continue ;
		status = fork_pipes(&it);
		dup2(g_global.fd[0], 0);
		dup2(g_global.fd[1], 1);
		dup2(g_global.fd[2], 2);
		if (g_global.in_cmd)
			g_global.cmd_ret = WEXITSTATUS(status);
	}
}

int	built_in(char **argv, int forked)
{
	if (*argv != NULL)
	{
		if (ft_str_equals(*argv, "echo"))
			do_echo(argv + 1);
		else if (ft_str_equals(*argv, "cd"))
			do_cd(argv + 1);
		else if (ft_str_equals(*argv, "pwd"))
			do_pwd(argv + 1);
		else if (ft_str_equals(*argv, "export"))
			do_export(argv + 1);
		else if (ft_str_equals(*argv, "unset"))
			do_unset(argv + 1);
		else if (ft_str_equals(*argv, "env"))
			do_env(argv + 1);
		else if (ft_str_equals(*argv, "exit"))
			do_exit(argv + 1);
		else
			return (FALSE);
		if (forked)
			exit(g_global.cmd_ret);
		return (TRUE);
	}
	return (FALSE);
}

int	main(int argc, char *argv[], char *envp[])
{
	int		prompt_size;

	if (argc != 1)
	{
		write(1, "Error: wrong number of arguments.\n", 34);
		return (1);
	}
	g_global.fd[0] = dup(0);
	g_global.fd[1] = dup(1);
	g_global.fd[2] = dup(2);
	(void)argv;
	prompt_size = initialize(envp);
	signal(SIGQUIT, signal_handler);
	signal(SIGWINCH, signal_handler);
	signal(SIGINT, signal_handler);
	terminal(prompt_size);
	return (0);
}
