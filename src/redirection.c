#include "minishell.h"

void	redirect_out(t_command *cmd)
{
	int		out_fd;
	int		pid;

	pid = fork();
	wait(NULL);
	if (pid == 0)
	{
		out_fd = open(cmd->redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (out_fd == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		if (dup2(out_fd, 1) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		if (close(out_fd) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		cmd_distributor((char **)as_array(cmd->args));
		exit(0);
	}
}

void	append(t_command *cmd)
{
	int		out_fd;
	int		pid;

	pid = fork();
	wait(NULL);
	if (pid == 0)
	{
		out_fd = open(cmd->redirect_out, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (out_fd == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
		if (dup2(out_fd, 1) == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
		if (close(out_fd) == -1)
		{
			ft_puterr2(">>: ", strerror(errno));
			return ;
		}
		cmd_distributor((char **)as_array(cmd->args));
		exit(0);
	}
}

void	redirect_in(t_command *cmd)
{
	int		in_fd;
	int		pid;

	pid = fork();
	wait(NULL);
	if (pid == 0)
	{
		in_fd = open(cmd->redirect_in, O_RDONLY);
		if (in_fd == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
		if (dup2(in_fd, 0) == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
		if (close(in_fd) == -1)
		{
			ft_puterr2("<: ", strerror(errno));
			return ;
		}
		cmd_distributor((char **)as_array(cmd->args));
		exit(0);
	}
}

void	piper(t_command *cmd)
{
	int		fd[2];
	int		pid;

	if (pipe(fd) == -1)
	{
		ft_puterr2("pipe: ", strerror(errno));
		return ;
	}
	pid = fork();
	if (pid != 0)
	{
		wait(NULL);
		if (dup2(fd[0], 0) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		close(fd[1]);
		cmd_distributor((char **)as_array(cmd->piper->args));
	}
	else
	{
		if (dup2(fd[1], 1) == -1)
		{
			ft_puterr2(">: ", strerror(errno));
			return ;
		}
		close(fd[0]);
		cmd_distributor((char **)as_array(cmd->args));
		exit(0);
	}
}

void	do_command(t_command *cmd)
{
	// t_list		*l;
	// t_command	p;

	// cmd->redirect_out = "test";
	// redirect_out(cmd);

	// cmd->redirect_in = "test";
	// redirect_in(cmd);

	// l = lst_new(NULL);
	// lst_push(l, "tail");
	// p.args = l;
	// cmd->piper = &p;
	// piper(cmd);

	cmd->redirect_out = "test";
	append(cmd);
}
