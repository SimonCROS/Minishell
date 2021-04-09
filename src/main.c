#include "minishell.h"

void	cmd_distributor(char **argv)
{
	if (*argv == NULL)
		return ;
	else if (!ft_strcmp(*argv, "echo"))
		do_echo(argv + 1);
	else if (!ft_strcmp(*argv, "cd"))
		do_cd(argv + 1);
	else if (!ft_strcmp(*argv, "pwd"))
		do_pwd(argv + 1);
	else if (!ft_strcmp(*argv, "export"))
		do_export(argv + 1);
	else if (!ft_strcmp(*argv, "unset"))
		do_unset(argv + 1);
	else if (!ft_strcmp(*argv, "env"))
		do_env(argv + 1);
	else if (!ft_strcmp(*argv, "exit"))
		do_exit(argv + 1);
	else
		do_execute(*argv, argv);
}

void	term_load(void)
{
	char			*term_name;
	struct termios	term;
	
	if (map_contains_key(global.env, "TERM"))
		term_name = map_get(global.env, "TERM");
	else
	{
		ft_putendl_fd("Impossible to determine the terminal\n", 2);
		exit(1);
	}
	if (tcgetattr(0, &term) == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		exit(1);
	}
	term.c_lflag &= ~(ECHO);
	term.c_lflag &= ~(ICANON);
	if (tcsetattr(0, TCSANOW, &term) == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		exit(1);
	}
	if (tgetent(NULL, term_name) != 1)
		exit(1);
}

char	*tgetval(char *id)
{
	char	*res;

	res = tgetstr(id, NULL);
	res[2] = '0';
	return (res);
}

void	test_adel(void)
{
	int			len;
	char		str[2000];
	// char		*buffer;
	// t_command	cmd;

	while (ft_strcmp(str, CTRL_D))
	{
		tputs(tgetstr("sc", NULL), 1, (int (*)(int))ft_putchar);
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			printf("0:%d 1:%d 2:%d 3:%d\n", *str, *(str + 1), *(str + 2), *(str + 3));
			printf("0:%d 1:%d 2:%d 3:%d\n", *key_up, *(key_up + 1), *(key_up + 2), *(key_up + 3));
			if (FALSE)
			{
				tputs(tgetstr("rc", NULL), 1, (int (*)(int))ft_putchar);
				tputs(tgetstr("ce", NULL), 1, (int (*)(int))ft_putchar);
			}
			else if (ft_str_equals(str, "\e[B"))
			{
				tputs(tgetstr("rc", NULL), 1, (int (*)(int))ft_putchar);
				tputs(tgetstr("ce", NULL), 1, (int (*)(int))ft_putchar);
			}
			else if (ft_str_equals(str, tgetstr("kb", NULL)))
				tputs(tgetstr("dc", NULL), 1, (int (*)(int))ft_putchar);
			else
				ft_putstr(str);
			if (ft_strcmp(str, "\n") && ft_strcmp(str, CTRL_D))
				break ;
		}
	}
	// result = 1;
	// while (result)
	// {
	// 	result = get_next_line(0, &buffer);
	// 	cmd.args = as_listf((void **)ft_split(buffer, ' '), free);
	// 	do_command(&cmd);
	// 	lst_destroy(cmd.args);
	// }
}

int	main(int argc, char *argv[], char *envp[])
{
	(void)argc;
	(void)argv;
	load_environment(envp);
	term_load();
	test_adel();
	return (0);
}
