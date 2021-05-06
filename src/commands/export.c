#include "minishell.h"

void	print_export(void)
{
	t_map			*sort;
	t_citerator		iter;
	t_mapentry		*elem;

	sort = map_sort(g_global.env, (t_com)ft_strcmp);
	iter = citerator_new((const t_clist *)sort);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		if (elem->value != NULL)
		{
			ft_putstr("declare -x ");
			ft_putstr(elem->key);
			ft_putstr("=\"");
			ft_putstr(elem->value);
			ft_putendl("\"");
		}
		else
		{
			ft_putstr("declare -x ");
			ft_putendl(elem->key);
		}
	}
	map_free(sort);
}

char	**split_in_half(char *str, char splitter)
{
	char	**res;
	char	*delim;

	res = malloc(sizeof(char *) * 3);
	res[2] = NULL;
	delim = ft_strchr(str, splitter);
	if (!delim)
	{
		res[0] = ft_strdup(str);
		res[1] = NULL;
		return (res);
	}
	*delim = 0;
	delim++;
	res[0] = ft_strdup(str);
	res[1] = ft_strdup(delim);
	return (res);
}

void	export_variable(char **argv, t_list *res, int i)
{
	if (!check_export_arg(res->first->value))
	{
		ft_puterr3("export: '", res->first->value, "': not a valid identifier");
		g_global.cmd_ret = NOT_VALID;
	}
	else if (res->size == 1 && argv[i][ft_strlen(res->first->value)] == '=' && \
	map_contains_key(g_global.env, res->first->value))
		map_replace(g_global.env, lst_shift(res), ft_strdup(""));
	else if (res->size == 1 && argv[i][ft_strlen(res->first->value)] == '=')
		map_put(g_global.env, lst_shift(res), ft_strdup(""));
	else if (res->size == 1 && map_contains_key(g_global.env, \
	res->first->value))
		;
	else if (res->size == 1)
		map_put(g_global.env, lst_shift(res), NULL);
	else if (res->size == 2 && map_contains_key(g_global.env, \
	res->first->value))
		map_replace(g_global.env, lst_shift(res), lst_shift(res));
	else if (res->size == 2)
		map_put(g_global.env, lst_shift(res), lst_shift(res));
	lst_destroy(res);
}

void	do_export(char **argv)
{
	t_list	*res;
	int		i;

	g_global.cmd_ret = 0;
	if (!*argv)
		print_export();
	i = -1;
	while (argv[++i])
	{
		res = as_listf((void **)split_in_half(argv[i], '='), free);
		if (res != NULL)
			export_variable(argv, res, i);
	}
}
