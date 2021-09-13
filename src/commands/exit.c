#include "minishell.h"

static int	strisnum(char *str)
{
	int		i;
	long	num;

	if (!str)
		return (TRUE);
	i = 0;
	num = 0;
	if (str[i] == '-' && str[1])
		i++;
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (FALSE);
		num = num * 10 + (-(str[i] - '0'));
		if (num > 0)
			return (FALSE);
		i++;
	}
	if (str[0] != '-' && num * -1 == num)
		return (FALSE);
	return (TRUE);
}

void	do_exit(char **argv)
{
	ft_putendl_fd("exit", 2);
	if (argv == NULL)
	{
		tcsetattr(0, TCSANOW, &g_global.save);
		exit(g_global.cmd_ret);
	}
	else if (!strisnum(argv[0]))
	{
		tcsetattr(0, TCSANOW, &g_global.save);
		ft_puterr3("exit: ", argv[0], ": numeric argument required");
		exit(255);
	}
	else if (argv[0] == NULL)
	{
		tcsetattr(0, TCSANOW, &g_global.save);
		exit(g_global.cmd_ret);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		ft_putendl_fd("exit: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	tcsetattr(0, TCSANOW, &g_global.save);
	exit((256 + ft_atoi(argv[0])) % 256);
}
