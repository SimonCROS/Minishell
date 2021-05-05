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
		ft_putendl_fd("minishell: Impossible to determine the terminal\n", 2);
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

void	signal_resize(void)
{
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
	tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
	tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
	tputs(clr_eos, 1, (int (*)(int))ft_putchar);
	ft_putstr(PROMPT);
	ft_putstr(*g_global.line);
}

void	signal_interupt(void)
{
	ft_putchar('\n');
	ft_putstr(PROMPT);
	tputs(save_cursor, 1, (int (*)(int))ft_putchar);
	free(*g_global.line);
	free(g_global.line);
	free(g_global.line_cpy);
	g_global.history = NULL;
	g_global.line_cpy = NULL;
	g_global.line = str_new();
	g_global.pos = 0;
}

void	signal_quit(void)
{
	ft_putstr("Quit: 3");
}

void	signal_handler(int sig)
{
	if (sig == SIGWINCH)
    	signal_resize();
	else if (sig == SIGQUIT)
		signal_quit();
	else if (sig == SIGINT)
		signal_interupt();
}

void	test_adel(int prompt_size)
{
	int				len;
	t_dlist			*history;
	char			str[2000];
	char			*clear;
	t_list			*cmds;

	history = dlst_new(free);
	clear = "clear";
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &g_global.wnsze);
	*str = 0;
	tputs(keypad_xmit, 1, (int (*)(int))ft_putchar);
	while (TRUE)
	{
		g_global.line_cpy = NULL;
		g_global.history = NULL;
		ft_putstr(PROMPT);
		tputs(save_cursor, 1, (int (*)(int))ft_putchar);
		g_global.line = str_new();
		g_global.pos = 0;
		tcsetattr(0, TCSANOW, &g_global.term);
		while (1)
		{
			len = read(1, str, 100);
			str[len] = 0;
			if (ft_str_equals(str, key_up))
			{
				if (!g_global.line_cpy)
					g_global.line_cpy = ft_strdup(*g_global.line);
				g_global.history = dlst_walk_right(g_global.history);
				if (!g_global.history)
					g_global.history = history->first;
				if (g_global.history)
				{
					free(*g_global.line);
					*g_global.line = ft_strdup(g_global.history->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*g_global.line);
				g_global.pos = ft_strlen(*g_global.line);
			}
			else if (ft_str_equals(str, key_down))
			{
				g_global.history = dlst_walk_left(g_global.history);
				if (!g_global.history)
				{
					if (g_global.line_cpy)
					{
						free(*g_global.line);
						*g_global.line = ft_strdup(g_global.line_cpy);
						free(g_global.line_cpy);
						g_global.line_cpy = NULL;
					}
					else
						tputs(bell, 1, (int (*)(int))ft_putchar);
				}
				else
				{
					free(*g_global.line);
					*g_global.line = ft_strdup(g_global.history->value);
				}
				tputs(restore_cursor, 1, (int (*)(int))ft_putchar);
				tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				ft_putstr(*g_global.line);
				g_global.pos = ft_strlen(*g_global.line);
			}
			else if (g_global.pos && (ft_str_equals(str, "\177") || ft_str_equals(str, key_backspace)))
			{
				if ((g_global.pos + prompt_size) % g_global.wnsze.ws_col == 0)
				{
					tputs(cursor_up, 1, (int (*)(int))ft_putchar);
					tputs(tgoto(tgetstr("ch", NULL), 0, g_global.wnsze.ws_col - 1), 1, (int (*)(int))ft_putchar);
					tputs(clr_eos, 1, (int (*)(int))ft_putchar);
				}
				else
					tputs(cursor_left, 1, (int (*)(int))ft_putchar);
				tputs(delete_character, 1, (int (*)(int))ft_putchar);
				g_global.line = str_rmlast(g_global.line);
				if (g_global.pos > 0)
					g_global.pos--;
			}
			else if (*str > 31 && *str < 127)
			{
				ft_putchar(*str);
				str_cappend(g_global.line, *str);
				if ((g_global.pos + prompt_size) % g_global.wnsze.ws_col == g_global.wnsze.ws_col - 1)
				{
					tputs(cursor_down, 1, (int (*)(int))ft_putchar);
					tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
				}
				g_global.pos++;
			}
			else if (*str == 12 && *(str + 1) == 0)
			{
				do_execute(clear, &clear);
				ft_putstr(PROMPT);
				tputs(save_cursor, 1, (int (*)(int))ft_putchar);
				ft_putstr(*g_global.line);
			}
			if (ft_str_equals(str, "\n") || (ft_str_equals(str, CTRL_D) && !g_global.pos))
				break ;
			if (ft_str_equals(str, CTRL_D))
				tputs(bell, 1, (int (*)(int))ft_putchar);
		}
		if (ft_str_equals(str, CTRL_D))
		{
			if (str_is_empty(*g_global.line) && g_global.pos == 0)
				do_exit(NULL);
			continue ;
		}
		ft_putchar('\n');
		free(g_global.line_cpy);
		if (!str_is_empty(*g_global.line))
			dlst_unshift(history, ft_strdup(*g_global.line));
		tcsetattr(0, TCSANOW, &g_global.save);
		cmds = parse_line(*g_global.line);
		if (!cmds)
			return ;
		do_command(cmds);
		lst_destroy(cmds);
		free(*g_global.line);
		free(g_global.line);
	}
}

int	main(int argc, char *argv[], char *envp[])
{
	int		prompt_size;

	(void)argc;
	(void)argv;
	g_global.fd[0] = dup(0);
	g_global.fd[1] = dup(1);
	g_global.cmd_ret = 0;
	prompt_size = ft_strlen(PROMPT);
	load_environment(envp);
	term_load();
	signal(SIGQUIT, signal_handler);
	signal(SIGWINCH, signal_handler);
	signal(SIGINT, signal_handler);
	test_adel(prompt_size);
	return (0);
}
