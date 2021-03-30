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
	env_path = ft_split(map_get(global.env, "PATH"), ':');
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

char	*env_compose(char *key, char *value)
{
	char	*res;
	int		key_size;
	int		value_size;

	key_size = ft_strlen(key);
	value_size = ft_strlen(value);
	res = malloc(key_size + value_size + 2);
	ft_strcpy(res, key);
	res[key_size] = '=';
	res[key_size + 1] = 0;
	ft_strlcat(res, value, key_size + value_size + 2);
	return (res);
}

char	**map_as_array(void)
{
	t_citerator		iter;
	t_mapentry		*elem;
	char			**array;
	int				i;

	i = 0;
	array = malloc(sizeof(char *) * (global.env->size + 1));
	iter = citerator_new((const t_clist *)global.env);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		array[i++] = env_compose(elem->key, elem->value);
	}
	array[i] = NULL;
	return (array);
}

void	do_execute(char *path, char **argv)
{
	int		pid;
	int		i;
	char	**env_path;
	char	**array;

	array = map_as_array();
	if (file_exists(path))
	{
		pid = fork();
		wait(NULL);
		if (pid == 0)
			if (execve(path, argv, array) == ERROR)
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
			if (execve(path, argv, array) == ERROR)
				printf("%s\n", strerror(errno));
		}
	}
	else
		printf("sh: command not found: %s\n", path);
	free(array);
	lst_destroy(as_listf((void**)env_path, free));
}
