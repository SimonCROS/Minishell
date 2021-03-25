#include "minishell.h"

int		file_exists(char *filename)
{
	struct stat		buffer;

	return (stat(filename, &buffer) == 0);
}

char	**get_env_path(char *path)
{
	int		i;
	char	**env_path;
	char	*new_path;
	char	*tmp;

	new_path = ft_strjoin("/", path);
	i = -1;
	while (ft_strncmp(global.envp[++i], "PATH", 4) != 0)
		;
	env_path = ft_split(global.envp[i] + 5, ':');
	i = -1;
	while (env_path[++i])
	{
		tmp = ft_strjoin(env_path[i], new_path);
		free(env_path[i]);
		env_path[i] = tmp;
	}
	free(new_path);
	return (env_path);
}

void	execute(char *path, char **argv)
{
	int		pid;
	int		i;
	char	**env_path;

	if (file_exists(path))
	{
		pid = fork();
		wait(NULL);
		if (pid == 0)
			if (execve(path, argv, global.envp) == ERROR)
				printf("%s\n", strerror(errno));
		return ;
	}
	env_path = get_env_path(path);
	i = -1;
	while (env_path[++i])
		if (file_exists(env_path[i]))
		{
			path = env_path[i];
			break ;
		}
	if (file_exists(env_path[i]))
	{
		pid = fork();
		wait(NULL);
		if (pid == 0)
		{
			if (execve(path, argv, global.envp) == ERROR)
				printf("%s\n", strerror(errno));
		}
	}
	else
		printf("sh: command not found: %s\n", path);
	lst_destroy(as_listf((void**)env_path, free));
}
