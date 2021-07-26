#include "minishell.h"

void	do_echo(char **argv)
{
	int		endl;

	endl = 1;
	while (*argv && ft_str_equals(*argv, "-n"))
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
