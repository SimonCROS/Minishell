#include "minishell.h"

int	redirect_out(t_command *cmd)
{
	int			out_fd;
	t_entry		*walk;
	t_redirect	*redirect;

	if (lst_is_empty(cmd->redirect_out))
		return (TRUE);
	walk = cmd->redirect_out->first;
	while (walk)
	{
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
		if (walk->next)
			close(out_fd);
		else
			dup2(out_fd, redirect->fd);
		walk = walk->next;
	}
	close(out_fd);
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
	dup2(in_fd, 0);
	close(in_fd);
	return (TRUE);
}

#include <stdio.h>
#include <sys/wait.h>

void	free_str_array(void **str_array)
{
	int	i;

	if (!str_array)
		return ;
	i = 0;
	while (str_array[i])
		free(str_array[i++]);
	free(str_array);
}

char	*get_path_from_env(char *path)
{
	t_list	*paths;
	char	*new_path;
	char	*result;
	char	*var;

	var = map_get(g_global.env, "PATH");
	if (!var)
	{
		errno = 2;
		ft_puterr3(path, ": ", strerror(errno));
		g_global.cmd_ret = errno;
		return (NULL);
	}
	new_path = ft_strjoin("/", path);
	paths = as_listf((void **)ft_split(var, ':'), free);
	lst_map_in(paths, (t_map_opts){ft_strjoin, new_path, 1}, free);
	result = ft_strdup(lst_find_first(paths, (t_pre)file_exists));
	free(new_path);
	lst_destroy(paths);
	return (result);
}

int	launch_command2(t_command *cmd)
{
	char	**argv;
	char	*path;

	if (!parse(cmd))
		exit(2);
	argv = (char **)as_array(cmd->args);
	path = get_path_from_env(argv[0]);
	if (argv && redirect_in(cmd) && redirect_out(cmd))
	{
		built_in(argv);
		if (!file_exists(path))
			exit(127);
		return (do_execute(path, argv));
	}
	exit(EXIT_FAILURE);
}

int	spawn_proc(int in, int out, t_command *cmd)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		exit(EXIT_FAILURE);
	if (pid)
	{
		if (in != 0)
		{
			dup2(in, 0);
			close(in);
		}
		if (out != 1)
		{
			dup2(out, 1);
			close(out);
		}
		return (launch_command2(cmd));
	}
	return (pid);
}

int	fork_pipes(t_iterator *it)
{
	t_command	*cmd;
	pid_t		pid;
	int			in;
	int			fd[2];

	in = 0;
	while (iterator_has_next(it))
	{
		cmd = iterator_next(it);
		if (cmd->next_relation != T_PIPE)
			break ;
		pipe(fd);
		puts("mêêêê");
		spawn_proc(in, fd[1], cmd);
		close(fd[1]);
		in = fd[0];
	}
	if (in != 0)
		dup2(in, 0);
	puts("hihan");
	return (launch_command2(cmd));
}

void	do_command(t_list *cmds)
{
	t_iterator	it;
	t_command	*cmd;
	int			status;
	int			pid;

	g_global.cmd_ret = 0;
	it = iterator_new(cmds);
	while (iterator_has_next(&it))
	{
		pid = fork();
		if (pid == -1)
			break ;
		if (!pid)
		{
			puts("miaw");
			fork_pipes(&it);
			exit(127);
		}
		puts("meuh");
		while (wait(&status) != pid)
			puts("aaa");
		puts("issou");
		while (((t_command *)iterator_next(&it))->next_relation == T_PIPE)
			;
		g_global.cmd_ret = WEXITSTATUS(status);
	}
}
