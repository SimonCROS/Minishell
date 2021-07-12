#include "minishell.h"

void	cd_update_env(int variable)
{
	if (variable == OLDPWD)
	{
		if (map_contains_key(g_global.env, "OLDPWD"))
			map_replace(g_global.env, "OLDPWD", \
			ft_strdup(getcwd(g_global.oldpwd, MAXPATHLEN)));
	}
	else if (variable == PWD)
	{
		if (map_contains_key(g_global.env, "PWD"))
			map_replace(g_global.env, "PWD", \
			ft_strdup(getcwd(g_global.pwd, MAXPATHLEN)));
	}
}

void	cd_redirect_home(void)
{
	char	path[MAXPATHLEN];
	char	*home;

	home = map_get(g_global.env, "HOME");
	if (home == NULL)
	{
		ft_puterr("cd: HOME not set");
		g_global.cmd_ret = NOT_SET;
		return ;
	}
	ft_strcpy(path, home);
	ft_strlcat(path, "/", MAXPATHLEN);
	g_global.cmd_ret = chdir(path);
}

void	do_cd(char **argv)
{
	g_global.cmd_ret = 0;
	cd_update_env(OLDPWD);
	if (argv[0] == NULL)
		cd_redirect_home();
	else if (argv[1] != NULL)
	{
		ft_puterr("cd: too many arguments");
		g_global.cmd_ret = TOO_MANY_ARGS;
	}
	else
		g_global.cmd_ret = chdir(argv[0]);
	if (g_global.cmd_ret == ERROR)
	{
		ft_puterr(strerror(errno));
		g_global.cmd_ret = 1;
	}
	cd_update_env(PWD);
}
