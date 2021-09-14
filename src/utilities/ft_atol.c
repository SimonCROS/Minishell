#include "minishell.h"

char	*skipspace(char *str)
{
	while (*str == 32)
		str++;
	return (str);
}

long	ft_atol(char *str)
{
	long	ret;
	int		mul;

	ret = 0;
	mul = 1;
	str = skipspace(str);
	if (*str == '-')
	{
		str++;
		mul = -1;
	}
	while (*str >= '0' && *str <= '9')
	{
		ret = ret * 10 + (*str - '0');
		str++;
	}
	return (ret * mul);
}
