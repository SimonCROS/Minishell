#include "minishell.h"

#include <stdio.h>

void	cmd_distributor(char **argv)
{
	if (*argv == NULL)
		return ;
	else if (!ft_strcmp(*argv, "pwd"))
		pwd(argv + 1);
	else if (!ft_strcmp(*argv, "cd"))
		cd(argv + 1);
	else
		execute(*argv, argv);
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
	global.envp = envp;
	test_adel();
	return (0);
}
