#include "minishell.h"

void	do_key_up(t_dlist *history)
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

void	do_key_down(void)
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

void	do_key_backspace(int prompt_size)
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

void	do_key_print(char *str, int prompt_size)
{
	ft_putchar(*str);
	str_cappend(g_global.line, *str);
	// if ((g_global.pos + prompt_size) % g_global.wnsze.ws_col == g_global.wnsze.ws_col - 1)
	// {
	// 	tputs(cursor_down, 1, (int (*)(int))ft_putchar);
	// 	tputs(tgetstr("cr", NULL), 1, (int (*)(int))ft_putchar);
	// }
	g_global.pos++;
}
