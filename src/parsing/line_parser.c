#include "minishell.h"

static char	*translate_var(char *str)
{
	if (!*str)
		return ("$");
	return (map_get(g_global.env, str));
}

static void	parse_variable2(t_list *var_tokens, t_token *token)
{
	t_token		*current;
	int			index;
	t_iterator	it;

	index = lst_index_of(token->parent->children, NULL, token);
	it = iterator_new(var_tokens);
	while (iterator_has_next(&it))
	{
		current = new_token(token->parent, T_WORD, NULL, FALSE);
		str_append(current->buffer, iterator_next(&it));
		lst_insert(token->parent->children, ++index, current);
		if (iterator_has_next(&it))
			lst_insert(token->parent->children, ++index,
				new_token(token->parent, T_WHITESPACE, NULL, FALSE));
	}
}

static void	parse_variable(t_token *token, char **container)
{
	char		exit_status[100];
	t_list		*var_tokens;

	if (**token->buffer == '?')
	{
		if (**token->buffer == '?')
			str_append(container, ft_itoa_to(g_global.cmd_ret, exit_status));
		else
			str_append(container, translate_var(*token->buffer));
		return ;
	}
	var_tokens = as_listf((void **)ft_split(
				translate_var(*token->buffer), ' '), free);
	if (!var_tokens)
		return ;
	parse_variable2(var_tokens, token);
	lst_destroy(var_tokens);
}

void	read_token(t_token *token, char **container)
{
	if (token->quoted)
	{
		if (token->children->size)
			lst_foreachp(token->children, (t_bicon)read_token, container);
		else
			str_append(container, "");
	}
	else if (token->type == T_VAR)
		parse_variable(token, container);
	else if (token->type == T_WHITESPACE)
		str_append(container, " ");
	else
		str_append(container, *token->buffer);
}

t_list	*parse_line(char *line)
{
	t_list	*tokens;
	t_list	*commands;
	t_token	empty;

	tokens = lst_new((t_con)free_token);
	commands = lst_new((t_con)free_command);
	if (!commands || !tokens)
		return (NULL);
	empty = null_token();
	empty.children = tokens;
	if (!tokenize(&empty, &line) || !validate(commands, tokens, 0, 0))
	{
		lst_destroy(tokens);
		lst_destroy(commands);
		return (NULL);
	}
	lst_destroy(tokens);
	return (commands);
}
