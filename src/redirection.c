#include "minishell.h"

static int	redirect_out2(t_entry *walk)
{
	t_redirect	*redirect;
	int			out_fd;

	redirect = ((t_redirect *)walk->value);
	if (redirect->append)
		out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		out_fd = open(redirect->str, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (out_fd == -1)
	{
		ft_puterr3(redirect->str, ": ", strerror(errno));
		g_global.cmd_ret = errno;
		return (FALSE);
	}
	if (!walk->next)
		dup2(out_fd, redirect->fd);
	close(out_fd);
	return (TRUE);
}

int	redirect_out(t_command *cmd)
{
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_out))
		return (TRUE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
		if (!redirect_out2(walk))
			return (FALSE);
		walk = walk->next;
	}
	return (TRUE);
}

int	redirect_in(t_command *cmd)
{
	int			in_fd;
	t_entry		*walk;

	if (lst_is_empty(cmd->redirect_in))
		return (TRUE);
	walk = cmd->redirect_in->first;
	while (walk)
	{
		in_fd = open(((t_redirect *)walk->value)->str, O_RDWR);
		if (in_fd == -1)
		{
			ft_puterr3(((t_redirect *)walk->value)->str, ": ", strerror(errno));
			g_global.cmd_ret = errno;
			return (FALSE);
		}
		if (walk->next)
			close(in_fd);
		walk = walk->next;
	}
	if (!walk)
		dup2(in_fd, 0);
	close(in_fd);
	return (TRUE);
}

static char	*get_path_from_env2(char *path)
{
	if (file_exists(path))
		return (path);
	errno = 2;
	ft_puterr3(path, ": ", strerror(errno));
	errno = 127;
	return (NULL);
}

char	*get_path_from_env(char *path)
{
	t_list	*paths;
	char	*new_path;
	char	*result;
	char	*var;

	if (!path)
		return (NULL);
	var = map_get(g_global.env, "PATH");
	if (!var || ft_strindex_of(path, '/') != -1)
		return (get_path_from_env2(path));
	new_path = ft_strjoin("/", path);
	paths = as_listf((void **)ft_split(var, ':'), free);
	lst_map_in(paths, (t_map_opts){ft_strjoin, new_path, 1}, free);
	result = ft_strdup(lst_find_first(paths, (t_pre)file_exists));
	free(new_path);
	lst_destroy(paths);
	if (!result)
	{
		errno = 127;
		ft_puterr3(path, ": ", "command not found");
		return (NULL);
	}
	return (result);
}
