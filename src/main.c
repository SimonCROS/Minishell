#include "minishell.h"

void	cmd_distributor(char **argv)
{
	if (*argv != NULL)
	{
		if (!ft_strcmp(*argv, "echo"))
			do_echo(argv + 1);
		else if (!ft_strcmp(*argv, "cd"))
			do_cd(argv + 1);
		else if (!ft_strcmp(*argv, "pwd"))
			do_pwd(argv + 1);
		else if (!ft_strcmp(*argv, "export"))
			do_export(argv + 1);
		else if (!ft_strcmp(*argv, "unset"))
			do_unset(argv + 1);
		else if (!ft_strcmp(*argv, "env"))
			do_env(argv + 1);
		else if (!ft_strcmp(*argv, "exit"))
			do_exit(argv + 1);
		else
			do_execute(*argv, argv);
	}
	free(argv);
}

int	main(int argc, char *argv[], char *envp[])
{
	int		prompt_size;

	(void)argc;
	(void)argv;
	prompt_size = initialize(envp);
	signal(SIGQUIT, signal_handler);
	signal(SIGWINCH, signal_handler);
	signal(SIGINT, signal_handler);
	terminal(prompt_size);
	return (0);
}
