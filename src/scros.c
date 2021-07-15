#include "minishell.h"

void	free_command(t_command *command)
{
	if (command)
	{
		lst_destroy(command->tokens);
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

	command = malloc(sizeof(t_command));
	if (!command)
		return (NULL);
	command->tokens = lst_new((t_consumer)free_token);
	command->args = lst_new(free);
	command->redirect_in = lst_new((t_con)free_redirect);
	command->redirect_out = lst_new((t_con)free_redirect);
	if (!command->tokens || !command->args || !command->redirect_in || !command->redirect_out)
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
			&& ft_strlen(*(token->buffer)) != 1)
			return (FALSE);
	if (token->type == T_REDIRECT_IN && ft_strlen(*(token->buffer)) > 1)
		return (FALSE);
	if (token->type == T_REDIRECT_OUT && ft_strlen(*(token->buffer)) > 2)
		return (FALSE);
	return (TRUE);
}

char	*parse_variable(char *str)
{
	char	exit_status[100];

	if (!*str)
		return ("$");
	if (str[0] == '?')
		return (ft_itoa_to(g_global.cmd_ret, exit_status));
	return (map_get(g_global.env, str));
}

int	parse_token(t_token *token, char **container)
{
	if (token->quoted)
	{
		if (token->children->size)
			lst_foreachp(token->children, (t_bicon)parse_token, container);
		else
			str_append(container, "");
	}
	else if (token->type == T_VAR)
	{
		// as_listf(ft_split())
		str_append(container, parse_variable(*(token->buffer)));
	}
	else
		str_append(container, *(token->buffer));
	return (TRUE);
}

int	validate(t_list *commands, t_list *tokens)
{
	char		*argument;
	t_token		*prev;
	t_token		*current;
	t_command	*command;
	int			space;
	int			started;

	started = 0;
	prev = NULL;
	space = 0;
	command = new_command(commands);
	argument = NULL;
	current = (t_token *)lst_shift(tokens);
	while (current || !started)
	{
		started = 1;
		lst_push(command->tokens, current);
		if (current->type == T_WHITESPACE)
			space = 1;
		else
		{
			if (!is_valid(current) || (current->separator && !prev) || (current->separator && prev->separator)
			|| (current->separator && prev->type == T_REDIRECT_IN) || (current->separator && prev->type == T_REDIRECT_OUT))
			{
				ft_puterr3("minish: syntax error near `", *(current->buffer), "'");
				return (FALSE);
			}
			if (current->separator)
			{
				command->next_relation = current->type;
				command = new_command(commands);
			}
			prev = current;
			space = 0;
		}
		current = (t_token *)lst_shift(tokens);
	}
	if (prev && (prev->type == T_REDIRECT_IN || prev->type == T_REDIRECT_OUT || prev->type == T_PIPE))
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
	t_iterator	tokens_iterator;
	int			space;
	t_list		*lst;
	int			append;

	space = 0;
	lst = command->args;
	tokens_iterator = iterator_new(command->tokens);
	argument = NULL;
	append = FALSE;
	while (iterator_has_next(&tokens_iterator))
	{
		current = (t_token *)iterator_next(&tokens_iterator);
		if (current->type == T_WHITESPACE)
		{
			space = 1;
			continue ;
		}
		if ((current->separator || current->type == T_REDIRECT_IN || current->type == T_REDIRECT_OUT || space) && argument)
		{
			if (lst == command->redirect_in || lst == command->redirect_out)
			{
				t_redirect	*redirection = malloc(sizeof(t_redirect));
				if (redirection)
					*redirection = (t_redirect){1, argument};
				lst_push(lst, redirection);
				redirection->append = append;
				append = FALSE;
			}
			else
				lst_push(lst, argument);
			argument = NULL;
			lst = command->args;
		}
		if (current->type == T_REDIRECT_OUT)
		{
			lst = command->redirect_out;
			append = ft_strlen(*(current->buffer)) == 2;
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
				*redirection = (t_redirect){1, argument};
			lst_push(lst, redirection);
			redirection->append = append;
			append = FALSE;
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
	if (!tokenize(&empty, &line) || !validate(commands, tokens))
	{
		lst_destroy(tokens);
		lst_destroy(commands);
		// printf("MEUH\n");
		return (NULL);
	}
	lst_destroy(tokens);
	return (commands);
}
