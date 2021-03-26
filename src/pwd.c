#include "minishell.h"

void	echo(char **argv)
{
	int		endl;

	endl = 1;
	if (*argv && !ft_strcmp(*argv, "-n"))
	{
		argv++;
		endl = 0;
	}
	while (*argv)
	{
		write(1, *argv, ft_strlen(*argv));
		if (*(argv + 1))
			write(1, " ", 1);
		argv++;
	}
	if (endl)
		write(1, "\n", 1);
}

void	cd(char **argv)
{
	int		ret;
	int		i;
	char	path[MAXPATHLEN];

	ret = 0;
	if (argv[0] == NULL)
	{
		i = -1;
		ft_strcpy(path, "/Users/");
		while (global.envp[++i])
			if (!ft_strncmp(global.envp[i], "USER=", 5))
			{
				ft_strlcat(path, global.envp[i] + 5, MAXPATHLEN);
				break ;
			}
		ret = chdir(path);
	}
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
