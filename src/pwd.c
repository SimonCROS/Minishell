#include "minishell.h"

void	cd(char **argv)
{
	int		ret;

	ret = 0;
	if (argv[0] == NULL)
		ret = chdir("~");
	else if (argv[1] != NULL)
		printf("cd: too many arguments\n");
	else
		ret = chdir(argv[0]);
	if (ret == ERROR)
		printf("%s\n", strerror(errno));
}

void	pwd(char **argv)
{
	char	*path;

	if (argv[0] != NULL)
	{
		printf("pwd: too many arguments\n");
		return ;
	}
	path = getcwd(global.pwd, MAXPATHLEN);
	if (path == NULL)
		printf("%s\n", strerror(errno));
	else
		printf("%s\n", path);
}
