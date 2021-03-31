#include "minishell.h"

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
		ft_strcpy(path, map_get(global.env, "HOME"));
		ft_strlcat(path, "/", MAXPATHLEN);
		ret = chdir(path);
	}
	else if (argv[1] != NULL)
		printf("cd: too many arguments\n");
	else
		ret = chdir(argv[0]);
	if (ret == ERROR)
		printf("%s\n", strerror(errno));
}

void	do_pwd(char **argv)
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

int		check_export_arg(char *arg)
{
	int		i;

	i = 0;
	if (!ft_isalpha(arg[i]))
		return (FALSE);
	while (arg[i])
		if (!ft_isalnum(arg[i++]))
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
		sort = map_sort(global.env, (t_com)ft_strcmp);
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
				printf("export: '%s': not a valid identifier\n", argv[i]);
			else if (res->size == 1 && map_contains_key(global.env, res->first->value))
				map_replace(global.env, lst_shift(res), NULL);
			else if (res->size == 1)
				map_put(global.env, lst_shift(res), NULL);
			else if (res->size >= 2 && map_contains_key(global.env, res->first->value))
				map_replace(global.env, lst_shift(res), lst_reduce(res, NULL, (t_bifun)env_compose, free));
			else if (res->size >= 2)
				map_put(global.env, lst_shift(res), lst_reduce(res, NULL, (t_bifun)env_compose, free));
			lst_destroy(res);
		}
	}
}

void	do_env(char **argv)
{
	t_citerator		iter;
	t_mapentry		*elem;

	if (*argv != NULL)
		printf("env: too many arguments\n");
	iter = citerator_new((const t_clist *)global.env);
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
			printf("export: '%s': not a valid identifier\n", argv[i]);
		else
			map_delete(global.env, argv[i]);
	}
}

void	do_exit(char **argv)
{
	printf("exit\n");
	if (argv[0] == NULL)
		exit(0);
	else if (!strisnum(argv[0]))
	{
		printf("exit: %s: numeric argument required\n", argv[0]);
		exit(255);
	}
	else if (argv[0] != NULL && argv[1] != NULL)
	{
		printf("exit: too many arguments\n");
		return ;
	}
	else 
		exit(ft_atoi(argv[0]));
}
