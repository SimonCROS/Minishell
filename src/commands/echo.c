#include "minishell.h"

int	echo_with_n(char *first_arg)
{
	int	i;

	if (*first_arg == '-')
	{
		i = 0;
		while (first_arg[++i])
			if (first_arg[i] != 'n')
				return (FALSE);
		return (i > 1);
	}
	return (FALSE);
}

void	do_echo(char **argv)
{
	int		endl;

	g_global.cmd_ret = 0;
	endl = 1;
	while (*argv && echo_with_n(*argv))
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
