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

void	do_echo(char **argv)
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

void	do_cd(char **argv)
{
	int		ret;
	int		i;
	char	path[MAXPATHLEN];

	ret = 0;
	if (argv[0] == NULL)
	{
		i = -1;
		ft_strcpy(path, map_get(g_global.env, "HOME"));
		ft_strlcat(path, "/", MAXPATHLEN);
		ret = chdir(path);
	}
	else if (argv[1] != NULL)
		ft_putendl_fd("cd: too many arguments", 2);
	else
		ret = chdir(argv[0]);
	if (ret == ERROR)
		ft_putendl_fd(strerror(errno), 2);
}

void	do_pwd(char **argv)
{
	char	*path;

	if (argv[0] != NULL)
	{
		ft_putendl_fd("pwd: too many arguments", 2);
		return ;
	}
	path = getcwd(g_global.pwd, MAXPATHLEN);
	ft_strlen(path);
	if (path == NULL)
		ft_putendl_fd(strerror(errno), 2);
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

void	do_export(char **argv)
{
	t_map			*sort;
	t_citerator		iter;
	t_mapentry		*elem;
	t_list			*res;
	int				i;

	i = -1;
	if (!*argv)
	{
		sort = map_sort(g_global.env, (t_com)ft_strcmp);
		iter = citerator_new((const t_clist *)sort);
		while (citerator_has_next(&iter))
		{
			elem = citerator_next(&iter);
			if (elem->value != NULL)
				printf("declare -x %s=\"%s\"\n", elem->key, elem->value);
			else
				printf("declare -x %s\n", elem->key);
		}
		map_free(sort);
	}
	while (argv[++i])
	{
		res = as_listf((void **)ft_split(argv[i], '='), free);
		if (res != NULL)
		{
			if (!check_export_arg(res->first->value))
				ft_puterr3("export: '", res->first->value, "': not a valid identifier");
			else if (res->size == 1 && argv[i][ft_strlen(argv[i]) - 1] == '=' && map_contains_key(g_global.env, res->first->value))
				map_replace(g_global.env, lst_shift(res), ft_strdup(""));
			else if (res->size == 1 && argv[i][ft_strlen(argv[i]) - 1] == '=')
				map_put(g_global.env, lst_shift(res), ft_strdup(""));
			else if (res->size == 1 && map_contains_key(g_global.env, res->first->value))
				map_replace(g_global.env, lst_shift(res), NULL);
			else if (res->size == 1)
				map_put(g_global.env, lst_shift(res), NULL);
			else if (res->size >= 2 && map_contains_key(g_global.env, res->first->value))
				map_replace(g_global.env, lst_shift(res), lst_reduce(res, NULL, (t_bifun)env_compose, free));
			else if (res->size >= 2)
				map_put(g_global.env, lst_shift(res), lst_reduce(res, NULL, (t_bifun)env_compose, free));
			lst_destroy(res);
		}
	}
}

void	do_env(char **argv)
{
	t_citerator		iter;
	t_mapentry		*elem;

	if (*argv != NULL)
		ft_putendl_fd("env: too many arguments", 2);
	iter = citerator_new((const t_clist *)g_global.env);
	while (citerator_has_next(&iter))
	{
		elem = citerator_next(&iter);
		if (elem->value != NULL)
			printf("%s=%s\n", elem->key, elem->value);
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

	i = -1;
	while (argv[++i])
	{
		if (!check_export_arg(argv[i]))
			ft_puterr3("export: '", argv[i], "': not a valid identifier");
		else
			map_delete(g_global.env, argv[i]);
	}
}

void	do_exit(char **argv)
{
	printf("exit\n");
	if (argv[0] == NULL)
		exit(0);
	else if (!strisnum(argv[0]))
	{
		ft_puterr3("exit: ", argv[0], ": numeric argument required");
		exit(255);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		ft_putendl_fd("exit: too many arguments", 2);
		return ;
	}
	else 
		exit(ft_atoi(argv[0]));
}
