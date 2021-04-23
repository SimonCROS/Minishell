#include "minishell.h"

void	free_map(char *key, char *value)
{
	free(key);
	free(value);
}

void	load_environment(char **envp)
{
	t_list    *res;
	void    *key;

	g_global.env = map_new((t_bipre)ft_str_equals, (t_bicon)free_map);
	while (*envp)
	{
		res = as_listf((void **)ft_split(*envp, '='), free);
		if (res != NULL)
		{
			if (res->size)
			{
				key = lst_shift(res);
				map_put(g_global.env, key, lst_reduce(res, NULL, (t_bifun)ft_strjoin, free));
			}
			lst_destroy(res);
		}
		envp++;
	}
}
