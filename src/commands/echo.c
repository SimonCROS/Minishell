#include "minishell.h"

int	echo_with_n(char *first_arg)
{
	if (*first_arg == '-')
	{
		++first_arg;
		while (*first_arg == 'n')
			++first_arg;
		if (!*first_arg)
			return (TRUE);
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
