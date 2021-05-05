#include "minishell.h"

void	ft_puterr2(char *a, char *b)
{
	ft_putstr_fd(a, 2);
	ft_putendl_fd(b, 2);
}

void	ft_puterr3(char *a, char *b, char *c)
{
	ft_putstr_fd(a, 2);
	ft_putstr_fd(b, 2);
	ft_putendl_fd(c, 2);
}

void	ft_puterr4(char *a, char *b, char *c, char *d)
{
	ft_putstr_fd(a, 2);
	ft_putstr_fd(b, 2);
	ft_putstr_fd(c, 2);
	ft_putendl_fd(d, 2);
}

void	do_echo(char **argv)
{
	int		endl;

	g_global.cmd_ret = 0;
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

void	do_cd(char **argv)
{
	int		i;
	char	path[MAXPATHLEN];
	char	*home;

	g_global.cmd_ret = 0;
	if (map_contains_key(g_global.env, "OLDPWD"))
		map_replace(g_global.env, "OLDPWD", ft_strdup(getcwd(g_global.oldpwd, MAXPATHLEN)));
	if (argv[0] == NULL)
	{
		i = -1;
		home = map_get(g_global.env, "HOME");
		if (home == NULL)
		{
			ft_putendl_fd("minishell: cd: HOME not set", 2);
			g_global.cmd_ret = NOT_SET;
			return ;
		}
		ft_strcpy(path, home);
		ft_strlcat(path, "/", MAXPATHLEN);
		g_global.cmd_ret = chdir(path);
	}
	else if (argv[1] != NULL)
	{
		ft_putendl_fd("minishell: cd: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
	}
	else
		g_global.cmd_ret = chdir(argv[0]);
	if (g_global.cmd_ret == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		g_global.cmd_ret = errno;
	}
	if (map_contains_key(g_global.env, "PWD"))
		map_replace(g_global.env, "PWD", ft_strdup(getcwd(g_global.pwd, MAXPATHLEN)));
}

void	do_pwd(char **argv)
{
	char	*path;

	g_global.cmd_ret = 0;
	if (argv[0] != NULL)
	{
		ft_putendl_fd("minishell: pwd: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	path = getcwd(g_global.pwd, MAXPATHLEN);
	ft_strlen(path);
	if (path == NULL)
	{
		ft_putendl_fd(strerror(errno), 2);
		g_global.cmd_ret = errno;
	}
	else
		ft_putendl(path);
}

int		check_export_arg(char *arg)
{
	int		i;

	i = -1;
	if (!ft_isalpha(arg[0]) && arg[0] != '_')
		return (FALSE);
	while (arg[++i])
		if (!ft_isalnum(arg[i]) && arg[i] != '_')
			return (FALSE);
	return (TRUE);
}

void	print_export(t_mapentry *elem)
{
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

char	**split_in_half(char *str, char splitter)
{
	char	**res;
	char	*delim;

	res = malloc(sizeof(char*) * 3);
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

void	do_export(char **argv)
{
	t_map			*sort;
	t_citerator		iter;
	t_mapentry		*elem;
	t_list			*res;
	int				i;

	g_global.cmd_ret = 0;
	i = -1;
	if (!*argv)
	{
		sort = map_sort(g_global.env, (t_com)ft_strcmp);
		iter = citerator_new((const t_clist *)sort);
		while (citerator_has_next(&iter))
		{
			elem = citerator_next(&iter);
			print_export(elem);
		}
		map_free(sort);
	}
	while (argv[++i])
	{
		res = as_listf((void **)split_in_half(argv[i], '='), free);
		if (res != NULL)
		{
			if (!check_export_arg(res->first->value))
			{
				ft_puterr3("minishell: export: '", res->first->value, "': not a valid identifier");
				g_global.cmd_ret = NOT_VALID;
			}
			else if (res->size == 1 && argv[i][ft_strlen(res->first->value)] == '=' && map_contains_key(g_global.env, res->first->value))
				map_replace(g_global.env, lst_shift(res), ft_strdup(""));
			else if (res->size == 1 && argv[i][ft_strlen(res->first->value)] == '=')
				map_put(g_global.env, lst_shift(res), ft_strdup(""));
			else if (res->size == 1 && map_contains_key(g_global.env, res->first->value))
				;
			else if (res->size == 1)
				map_put(g_global.env, lst_shift(res), NULL);
			else if (res->size == 2 && map_contains_key(g_global.env, res->first->value))
				map_replace(g_global.env, lst_shift(res), lst_shift(res));
			else if (res->size == 2)
				map_put(g_global.env, lst_shift(res), lst_shift(res));
			lst_destroy(res);
		}
	}
}

void	do_env(char **argv)
{
	t_citerator		iter;
	t_mapentry		*elem;

	g_global.cmd_ret = 0;
	if (*argv != NULL)
	{
		ft_putendl_fd("minishell: env: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	iter = citerator_new((const t_clist *)g_global.env);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		if (elem->value != NULL)
		{
			ft_putstr(elem->key);
			ft_putchar('=');
			ft_putendl(elem->value);
		}
	}
}

int		strisnum(char *str)
{
	int		i;

	i = 0;
	while (str[i])
		if (!ft_isdigit(str[i++]))
			return (FALSE);
	return (TRUE);
}

void	do_unset(char **argv)
{
	int		i;

	g_global.cmd_ret = 0;
	i = -1;
	while (argv[++i])
	{
		if (!check_export_arg(argv[i]))
		{
			ft_puterr3("minishell: export: '", argv[i], "': not a valid identifier");
			g_global.cmd_ret = NOT_VALID;
		}
		else
		{
			map_delete(g_global.env, argv[i]);
			g_global.cmd_ret = 0;
		}
	}
}

void	do_exit(char **argv)
{
	ft_putendl_fd("exit", 1);
	if (argv == NULL)
		exit(0);
	if (argv[0] == NULL)
		exit(g_global.cmd_ret);
	else if (!strisnum(argv[0]))
	{
		ft_puterr3("minishell: exit: ", argv[0], ": numeric argument required");
		exit(255);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		ft_putendl_fd("minishell: exit: too many arguments", 2);
		g_global.cmd_ret = TOO_MANY_ARGS;
		return ;
	}
	else 
		exit(ft_atoi(argv[0]));
}
