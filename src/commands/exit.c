#include "minishell.h"

int	strisnum(char *str)
{
	int		i;

	i = 0;
	while (str[i])
		if (!ft_isdigit(str[i++]))
			return (FALSE);
	return (TRUE);
}

void	do_exit(char **argv)
{
	ft_putendl_fd("exit", 1);
	if (argv == NULL)
		exit(0);
	if (argv[0] == NULL)
		exit(g_global.cmd_ret);
	else if (!strisnum(argv[0]))
	{
		ft_puterr3("minishell: exit: ", argv[0], ": numeric argument required");
		exit(255);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	else
		exit(ft_atoi(argv[0]));
}
