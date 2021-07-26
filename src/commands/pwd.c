#include "minishell.h"

void	do_pwd(char **argv)
{
	char	*path;

	if (argv[0] != NULL)
	{
		ft_puterr("pwd: too many arguments");
		g_global.cmd_ret = TOO_MANY_ARGS;
	}
	path = getcwd(g_global.pwd, MAXPATHLEN);
	if (path == NULL)
		ft_putendl(g_global.pwd);
	else
		ft_putendl(path);
}
