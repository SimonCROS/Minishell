#include "minishell.h"

void	do_pwd(char **argv)
{
	char	*path;

	if (argv[0] != NULL)
	{
		ft_puterr("pwd: too many arguments");
		exit(TOO_MANY_ARGS);
	}
	path = getcwd(g_global.pwd, MAXPATHLEN);
	ft_strlen(path);
	if (path == NULL)
	{
		ft_puterr(strerror(errno));
		exit(errno);
	}
	else
		ft_putendl(path);
}
