#include "minishell.h"

void	do_echo(char **argv)
{
	int		endl;

	g_global.cmd_ret = 0;
	endl = 1;
	if (*argv && !ft_strcmp(*argv, "-n"))
	{
		argv++;
		endl = 0;
	}
	while (*argv)
	{
		ft_putstr(*argv);
		if (*(argv + 1))
			ft_putchar(' ');
		argv++;
	}
	if (endl)
		ft_putchar('\n');
}
