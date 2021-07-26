#include "minishell.h"

static void	print_export(void)
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

static void	export_variable(char *str)
{
	int		append;
	int		len;
	char	*value;

	append = 0;
	len = ft_strindex_of(str, '=');
	value = str + len + 1;
	append = len > 1 && str[len - 1] == '+';
	if (append)
		len--;
	str[len] = 0;
	if (!check_export_arg(str))
	{
		ft_puterr3("export: '", str, "': not a valid identifier");
		g_global.cmd_ret = NOT_VALID;
		return ;
	}
	if (len != -1)
	{
		if (map_contains_key(g_global.env, str))
		{
			if (!append)
				map_replace(g_global.env, ft_substr(str, 0, len), ft_strdup(value));
			else
			{
				if (!map_get(g_global.env, str))
					map_replace(g_global.env, ft_substr(str, 0, len), ft_strdup(value));
				else
					map_replace(g_global.env, ft_substr(str, 0, len), ft_strjoin(map_get(g_global.env, str), value));
			}
		}
		else
			map_put(g_global.env, ft_substr(str, 0, len), ft_strdup(value));
	}
	else
		if (!map_contains_key(g_global.env, str))
			map_put(g_global.env, ft_substr(str, 0, len), NULL);
}

void	do_export(char **argv)
{
	int		i;

	if (!*argv)
		print_export();
	i = -1;
	while (argv[++i])
		export_variable(argv[i]);
}
