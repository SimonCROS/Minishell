#include "minishell.h"

void	do_pwd(char **argv)
{
	char	*path;

	g_global.cmd_ret = 0;
	if (argv[0] != NULL)
	{
		ft_puterr("pwd: too many arguments");
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	path = getcwd(g_global.pwd, MAXPATHLEN);
	ft_strlen(path);
	if (path == NULL)
	{
		ft_puterr(strerror(errno));
		g_global.cmd_ret = errno;
	}
	else
		ft_putendl(path);
}
