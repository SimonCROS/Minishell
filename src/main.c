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
	
	if (map_contains_key(g_global.env, "TERM"))
		term_name = map_get(g_global.env, "TERM");
	else
	{
		ft_putendl_fd("Impossible to determine the terminal\n", 2);
		exit(1);
	}
	if (tcgetattr(0, &g_global.term) == ERROR || tcgetattr(0, &g_global.save) == ERROR)
	{
		ft_putendl_fd(strerror(errno), 2);
		exit(1);
	}
	g_global.term.c_lflag &= ~(ICANON | ECHO);
	g_global.term.c_cc[VMIN] = 1;
    g_global.term.c_cc[VTIME] = 0;
	if (tcsetattr(0, TCSANOW, &g_global.term) == ERROR)
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
	int			pos;
	int			count;
	int			len;
	t_list		*history;
	char		str[2000];
	char		**line;
	t_command	*cmd;

	history = lst_new(free);
	count = 0;
	*str = 0;
	pos = 0;
	tputs(keypad_xmit, 1, (int (*)(int))ft_putchar);
	while (!ft_str_equals(str, CTRL_D))
	{
		tputs(save_cursor, 1, (int (*)(int))ft_putchar);
		line = str_new();
		tcsetattr(0, TCSANOW, &g_global.term);
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			if (ft_str_equals(str, key_up))
			{
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				free(*line);
	 			*line = ft_strdup("");
			}
			else if (ft_str_equals(str, key_down))
			{
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				free(*line);
	 			*line = ft_strdup("");
			}
			else if (ft_str_equals(str, (char[2]){ 127, 0 }) || ft_str_equals(str, key_backspace))
			{
				tputs(cursor_left, 1, (int (*)(int))ft_putchar);
				tputs(delete_character, 1, (int (*)(int))ft_putchar);
				line = str_rmlast(line);
			}
			else if (*str > 31 && *str < 127)
			{
				ft_putchar(*str);
				str_cappend(line, *str);
			}
			if (ft_str_equals(str, "\n") || ft_str_equals(str, CTRL_D))
				break ;
		}
		ft_putchar('\n');
		tcsetattr(0, TCSANOW, &g_global.save);
		cmd = lst_first(parse_line(*line));
		free(*line);
		if (cmd)
			cmd_distributor((char **)as_array(cmd->args));
		free(line);
	}
	// while (ft_strcmp(str, CTRL_D))
	// {
	// 	line = str_new();
	// 	tcsetattr(0, TCSANOW, &global.shell.term);
	// 	curr = count;
	// 	while (1)
	// 	{
	// 		len = read(1, str, 100);
	// 		str[len] = 0;
	// 		if (ft_str_equals(str, "\e[A"))
	// 		{
	// 			tputs(tgetstr("dl", NULL), 1, (int (*)(int))ft_putchar);
	// 			tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
	// 			free(*line);
	// 			*line = ft_strdup("");
	// 			lst_get(history, curr);
	// 		}
	// 		else if (ft_str_equals(str, "\e[B"))
	// 		{
	// 			tputs(tgetstr("dl", NULL), 1, (int (*)(int))ft_putchar);
	// 			tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
	// 			free(*line);
	// 			*line = ft_strdup("");
	// 		}
	// 		else if (*str == 127 && *(str + 1) == 0)
	// 		{
	// 			tputs(tgetstr("le", NULL), 1, (int (*)(int))ft_putchar);
	// 			tputs(tgetstr("dc", NULL), 1, (int (*)(int))ft_putchar);
	// 			line = str_rmlast(line);
	// 		}
	// 		else if (*str > 31 && *str < 127)
	// 		{
	// 			ft_putchar(*str);
	// 			str_cappend(line, *str);
	// 		}
	// 		if (!ft_strcmp(str, "\n") || !ft_strcmp(str, CTRL_D))
	// 			break ;
	// 	}
	// 	ft_putchar('\n');
	// 	if (!ft_str_equals(*line, "\n"))
	// 	{
	// 		lst_push(history, *line);
	// 		count++;
	// 	}
	// 	tcsetattr(0, TCSANOW, &global.shell.save);
	// 	cmd.args = as_listf((void **)ft_split(*line, ' '), free);
	// 	free(*line);
	// 	cmd_distributor((char **)as_array(cmd.args));
	// 	lst_destroy(cmd.args);
	// 	free(line);
	// }
	// while (1)
	// {
	// 	get_next_line(0, &buffer);
	// 	cmd.args = as_listf((void **)ft_split(buffer, ' '), free);
	// 	// do_command(&cmd);
	// 	cmd_distributor(as_array(cmd.args));
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

// a"b'c'd"e'f"g"h'i|<>\|\<\>&&\&\&;\;
