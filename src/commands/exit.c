#include "minishell.h"

static int	isnum_loop(char *str, int *i, unsigned long *num, int *c)
{
	if (str[*i] == 32)
	{
		*c = (*i)++;
		return (TRUE);
	}
	if (!ft_isdigit(str[*i]) || *c > 0)
		return (FALSE);
	*num = *num * 10 + (str[(*i)++] - '0');
	if ((str[0] == '-' && str[1] == 32) || (str[0] != '-' && *num > LONG_MAX)
		|| (str[0] == '-' && *num >= LONG_MAX))
		return (FALSE);
	return (TRUE);
}

static int	strisnum(char *str)
{
	int				i;
	int				checker;
	unsigned long	num;

	if (!str)
		return (TRUE);
	i = 0;
	num = 0;
	checker = 0;
	str = skipspace(str);
	if (str[i] == '-' && str[i + 1])
		i++;
	while (str[i])
	{
		if (isnum_loop(str, &i, &num, &checker))
			continue ;
		else
			return (FALSE);
	}
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
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	tcsetattr(0, TCSANOW, &g_global.save);
	exit((256 + ft_atol(argv[0])) % 256);
}
