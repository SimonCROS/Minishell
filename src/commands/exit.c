#include "minishell.h"

static int	strisnum(char *str)
{
	long	mul;
	long	num;

	mul = 1;
	str = skipspace(str);
	if (*str == '-')
	{
		str++;
		mul = -1;
	}
	if (!*str)
		return (FALSE);
	num = 0;
	while (*str >= '0' && *str <= '9')
	{
		num = num * 10 + (*str - '0') * mul;
		if ((num < 0 && mul == 1) || (num > 0 && mul == -1))
			return (FALSE);
		str++;
	}
	str = skipspace(str);
	return (!*str);
}

void	do_exit(char **argv, int forked)
{
	if (!forked)
		ft_putendl_fd("exit", 2);
	if (argv == NULL || argv[0] == NULL)
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
	else if (argv[1] != NULL)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	tcsetattr(0, TCSANOW, &g_global.save);
	exit((256 + ft_atol(argv[0])) % 256);
}
