#include "minishell.h"

static t_command	*new_command(t_list *commands)
{
	t_command	*command;

	command = ft_calloc(1, sizeof(t_command));
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

static int	is_valid(t_token *token)
{
	if (!token)
		return (FALSE);
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

int	is_redirection(t_list *tokens, t_token *current)
{
	if (!current)
		return (FALSE);
	if (current->type == T_REDIRECT_IN)
		return (TRUE);
	else if (current->type == T_REDIRECT_OUT)
		return (TRUE);
	else if (tokens && tokens->first && current->type == T_NUMBER
		&& ((t_token *)lst_first(tokens))->type == T_REDIRECT_OUT)
		return (TRUE);
	return (FALSE);
}

static int	run_test(t_command *command, t_list *tokens, t_token *cur,
	t_token *prev)
{
	cur->parent = (t_token *)command;
	lst_push(command->children, cur);
	if (cur->type != T_WHITESPACE)
	{
		if (!is_valid(cur) || (cur->separator && !prev) || (cur->separator \
		&& prev->separator) || (cur->separator && prev->type == \
		T_REDIRECT_IN) || (cur->separator && prev->type == T_REDIRECT_OUT) \
		|| (is_redirection(tokens, prev) && is_redirection(tokens, cur)))
		{
			ft_puterr3("minish: syntax error near `", *(cur->buffer), "'");
			return (-1);
		}
		if (cur->separator)
		{
			command->next_relation = cur->type;
			return (2);
		}
		return (1);
	}
	return (0);
}

int	validate(t_list *commands, t_list *tokens, int started, int test_result)
{
	t_token		*prev;
	t_token		*cur;
	t_command	*command;

	prev = NULL;
	command = new_command(commands);
	cur = (t_token *)lst_shift(tokens);
	while (cur || !started)
	{
		started = 1;
		test_result = run_test(command, tokens, cur, prev);
		if (test_result == -1)
			return (FALSE);
		if (test_result > 0)
			prev = cur;
		if (test_result == 2)
			command = new_command(commands);
		cur = (t_token *)lst_shift(tokens);
	}
	if (prev && (is_redirection(NULL, prev) || prev->type == T_PIPE))
	{
		ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
		return (FALSE);
	}
	return (TRUE);
}
