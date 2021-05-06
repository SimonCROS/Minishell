#include "minishell.h"

char	**get_env_path(char *path)
{
	int		i;
	char	**env_path;
	char	*new_path;
	char	*tmp;
	char	*val;

	val = map_get(g_global.env, "PATH");
	if (!val)
	{
		errno = 2;
		ft_puterr3(path, ": ", strerror(errno));
		g_global.cmd_ret = errno;
		return (NULL);
	}
	new_path = ft_strjoin("/", path);
	env_path = ft_split(val, ':');
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

	if (key == NULL)
		return (ft_strdup(value));
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
	array = malloc(sizeof(char *) * (g_global.env->size + 1));
	iter = citerator_new((const t_clist *)g_global.env);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		if (elem->value)
			array[i++] = env_compose(elem->key, elem->value);
		else
			array[i++] = ft_strdup(elem->key);
	}
	array[i] = NULL;
	return (array);
}

void	execute_from_path(char *path, char **env_path, char **argv, char **env)
{
	int		i;
	int		pid;

	i = -1;
	while (env_path[++i])
	{
		if (file_exists(env_path[i]))
		{
			path = env_path[i];
			break ;
		}
	}
	if (file_exists(env_path[i]))
	{
		pid = fork();
		wait(NULL);
		if (pid == 0 && execve(path, argv, env) == ERROR)
			execve_err();
	}
	else
	{
		ft_puterr2("command not found: ", path);
		g_global.cmd_ret = errno;
	}
}

void	do_execute(char *path, char **argv)
{
	int		pid;
	int		i;
	char	**env_path;
	char	**array;

	g_global.cmd_ret = 0;
	array = map_as_array();
	if (file_exists(path))
	{
		pid = fork();
		wait(NULL);
		if (pid == 0 && execve(path, argv, array) == ERROR)
			execve_err();
		return ;
	}
	env_path = get_env_path(path);
	if (!env_path)
		return ;
	execute_from_path(path, env_path, argv, array);
	i = -1;
	while (array[++i])
		free(array[i]);
	free(array);
	lst_destroy(as_listf((void **)env_path, free));
}
