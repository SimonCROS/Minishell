#include "minishell.h"

void	free_command(t_command *command)
{
	if (command)
	{
		lst_destroy(command->children);
		lst_destroy(command->args);
		lst_destroy(command->redirect_out);
		lst_destroy(command->redirect_in);
	}
	free(command);
}

int	token_equals(t_token *t1, t_token *t2)
{
	if (!t1 || !t2 || !t1->buffer || !t2->buffer)
		return (FALSE);
	return (t1->quoted == t2->quoted && t1->type == t2->type
		&& ft_str_equals(*(t1->buffer), *(t2->buffer)));
}

void	free_redirect(t_redirect *redirect)
{
	if (redirect)
		free(redirect->str);
	free(redirect);
}

t_command	*new_command(t_list *commands)
{
	t_command	*command;

	command = calloc(1, sizeof(t_command));
	if (!command)
		return (NULL);
	command->children = lst_new((t_consumer)free_token);
	command->args = lst_new(free);
	command->redirect_in = lst_new((t_con)free_redirect);
	command->redirect_out = lst_new((t_con)free_redirect);
	if (!command->children || !command->args || !command->redirect_in \
	|| !command->redirect_out)
	{
		free_command(command);
		return (NULL);
	}
	command->next_relation = T_NONE;
	if (!commands)
		return (command);
	if (!lst_push(commands, command))
	{
		free_command(command);
		return (NULL);
	}
	return (command);
}

// To protect

int	is_valid(t_token *token)
{
	if (token->separator)
		if ((token->type == T_SEPARATOR || token->type == T_PIPE)
			&& ft_strlen(*token->buffer) != 1)
			return (FALSE);
	if (token->type == T_REDIRECT_IN && ft_strlen(*token->buffer) > 1)
		return (FALSE);
	if (token->type == T_REDIRECT_OUT && ft_strlen(*token->buffer) > 2)
		return (FALSE);
	return (TRUE);
}

char	*translate_var(char *str)
{
	if (!*str)
		return ("$");
	return (map_get(g_global.env, str));
}

static void	parse_variable(t_token *token, char **container)
{
	char		exit_status[100];
	t_list		*var_tokens;
	t_token		*current;
	int			index;
	t_iterator	it;

	if (**token->buffer == '?')
	{
		ft_itoa_to(g_global.cmd_ret, exit_status);
		str_append(container, exit_status);
		return ;
	}
	if (token->parent->quoted)
	{
		str_append(container, translate_var(*token->buffer));
		return ;
	}
	var_tokens = as_listf((void **)ft_split(
				translate_var(*token->buffer), ' '), free);
	if (!var_tokens)
		return ;
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

void	parse_token(t_token *token, char **container)
{
	if (token->quoted)
	{
		if (token->children->size)
			lst_foreachp(token->children, (t_bicon)parse_token, container);
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

int	validate(t_list *commands, t_list *tokens, int started)
{
	char		*argument;
	t_token		*prev;
	t_token		*cur;
	t_command	*command;
	int			space;

	prev = NULL;
	space = 0;
	command = new_command(commands);
	argument = NULL;
	cur = (t_token *)lst_shift(tokens);
	while (cur || !started)
	{
		started = 1;
		cur->parent = (t_token *)command;
		lst_push(command->children, cur);
		if (cur->type == T_WHITESPACE)
			space = 1;
		else
		{
			if (!is_valid(cur) || (cur->separator && !prev) || (cur->separator \
			&& prev->separator) || (cur->separator && prev->type == \
			T_REDIRECT_IN) || (cur->separator && prev->type == T_REDIRECT_OUT))
			{
				ft_puterr3("minish: syntax error near `", *(cur->buffer), "'");
				return (FALSE);
			}
			if (cur->separator)
			{
				command->next_relation = cur->type;
				command = new_command(commands);
			}
			prev = cur;
			space = 0;
		}
		cur = (t_token *)lst_shift(tokens);
	}
	if (prev && (prev->type == T_REDIRECT_IN || prev->type == T_REDIRECT_OUT \
	|| prev->type == T_PIPE))
	{
		ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
		return (FALSE);
	}
	return (TRUE);
}

int	parse(t_command *command)
{
	char		*argument;
	t_token		*current;
	t_token		*next;
	t_iterator	it;
	int			space;
	t_list		*lst;
	int			append;
	int			fd;

	space = 0;
	lst = command->args;
	it = iterator_new(command->children);
	argument = NULL;
	append = FALSE;
	fd = 1;
	next = (t_token *)iterator_next(&it);
	while (next)
	{
		current = next;
		next = (t_token *)iterator_next(&it);
		if (current->type == T_WHITESPACE)
		{
			space = 1;
			continue ;
		}
		if (current->type == T_NUMBER && next && next->type == T_REDIRECT_OUT)
		{
			fd = ft_atoi(*current->buffer);
			continue ;
		}
		if ((current->separator || current->type == T_REDIRECT_IN || \
		current->type == T_REDIRECT_OUT || space) && argument)
		{
			if (lst == command->redirect_in || lst == command->redirect_out)
			{
				t_redirect	*redirection = malloc(sizeof(t_redirect));
				if (redirection)
					*redirection = (t_redirect){fd, argument, append};
				lst_push(lst, redirection);
				append = FALSE;
				fd = 1;
			}
			else
				lst_push(lst, argument);
			argument = NULL;
			lst = command->args;
		}
		if (current->type == T_REDIRECT_OUT)
		{
			lst = command->redirect_out;
			append = ft_strlen(*current->buffer) == 2;
		}
		else if (current->type == T_REDIRECT_IN)
			lst = command->redirect_in;
		else if (!current->separator)
			parse_token(current, &argument);
		space = 0;
	}
	if (argument)
	{
		if (lst == command->redirect_in || lst == command->redirect_out)
		{
			t_redirect	*redirection = malloc(sizeof(t_redirect));
			if (redirection)
				*redirection = (t_redirect){fd, argument, append};
			lst_push(lst, redirection);
			append = FALSE;
			fd = 1;
		}
		else
			lst_push(lst, argument);
	}
	return (TRUE);
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
	if (!tokenize(&empty, &line) || !validate(commands, tokens, 0))
	{
		lst_destroy(tokens);
		lst_destroy(commands);
		return (NULL);
	}

	lst_destroy(tokens);
	return (commands);
}
