#include "minishell.h"


void	cmd_distributor(char **argv)
{
	if (*argv == NULL)
		return ;
	else if (!ft_strcmp(*argv, "echo"))
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

void	test_adel(void)
{
	int			result;
	char		*buffer;
	char		**argv;

	result = 1;
	while (result)
	{
		result = get_next_line(0, &buffer);
		argv = ft_split(buffer, ' ');
		free(buffer);
		cmd_distributor(argv);
		lst_destroy(as_listf((void**)argv, free));
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	load_environment(envp);
	test_adel();
	return (0);
}
