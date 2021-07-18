#include "minishell.h"

char	*get_path_from_env(char *path)
{
	t_list	*paths;
	char	*new_path;
	char	*result;
	char	*var;

	var = map_get(g_global.env, "PATH");
	if (!var)
	{
		errno = 2;
		ft_puterr3(path, ": ", strerror(errno));
		g_global.cmd_ret = errno;
		return (NULL);
	}
	new_path = ft_strjoin("/", path);
	paths = as_listf(ft_split(var, ':'), free);
	lst_map_in(paths, (t_map_opts){{ft_strjoin}, new_path, 1}, free);
	result = lst_find_first(paths, file_exists);
	free(new_path);
	lst_destroy(paths);
	return (result);
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

void	free_str_array(void **str_array)
{
	int	i;

	if (!str_array)
		return ;
	i = 0;
	while (str_array[i])
		free(str_array[i++]);
	free(str_array);
}

void	do_execute(char *path, char **argv)
{
	int		i;
	char	**array;

	g_global.cmd_ret = 0;
	array = map_as_array();
	if (ft_strindex_of(path, '/') != -1 && file_exists(path))
	{
		execve(path, argv, array);
		return ;
	}
	path = get_path_from_env(path);
	if (!path)
		return ;
	if (ft_strindex_of(path, '/') != -1 && file_exists(path))
		execve(path, argv, array);
	free_str_array(argv);
	free(path);
}
