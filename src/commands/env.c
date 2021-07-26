#include "minishell.h"

void	do_env(char **argv)
{
	t_citerator		iter;
	t_mapentry		*elem;

	if (*argv != NULL)
	{
		ft_putendl_fd("minishell: env: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	iter = citerator_new((const t_clist *)g_global.env);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		if (elem->value != NULL)
		{
			ft_putstr(elem->key);
			ft_putchar('=');
			ft_putendl(elem->value);
		}
	}
}
