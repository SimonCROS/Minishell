#include "minishell.h"

int	check_export_arg(char *arg)
{
	int		i;

	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (FALSE);
	i = 0;
	while (arg[i])
	{
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (FALSE);
		i++;
	}
	return (TRUE);
}

void	do_unset(char **argv)
{
	int		i;

	g_global.cmd_ret = 0;
	i = -1;
	while (argv[++i])
	{
		if (!check_export_arg(argv[i]))
		{
			ft_puterr3("minishell: export: '", argv[i], \
			"': not a valid identifier");
			g_global.cmd_ret = NOT_VALID;
		}
		else
			map_delete(g_global.env, argv[i]);
	}
}
