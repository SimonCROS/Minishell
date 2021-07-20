#include "minishell.h"

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

void	do_execute(char *path, char **argv)
{
	int		i;
	char	**array;

	if (!file_exists(path))
		return ;
	array = map_as_array();
	execve(path, argv, array);
	free(path);
}
