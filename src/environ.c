#include "minishell.h"

void	free_map(char *key, char *value)
{
	free(key);
	free(value);
}

void	load_environment(char **envp)
{
	t_list	*res;

	global.env = map_new((t_bipre)ft_str_equals, (t_bicon)free_map);
	while (*envp)
	{
		res = as_listf((void **)ft_split(*envp, '='), free);
		if (res != NULL)
		{
			if (res->size)
				map_put(global.env, lst_shift(res), lst_reduce(res, NULL, (t_bifun)ft_strjoin, free));
			lst_destroy(res);
		}
		envp++;
	}
}
