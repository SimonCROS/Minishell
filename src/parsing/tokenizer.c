#include "minishell.h"

#include <stdio.h>

void	free_token(t_token *token)
{
	if (token)
	{
		if (token->buffer)
			free(*(token->buffer));
		free(token->buffer);
	}
	free(token);
}

t_token	null_token(void)
{
	return ((t_token){NULL, T_NONE, FALSE, FALSE});
}

static t_token	*new_token(t_list *tokens, t_token_type type, t_token *cur)
{
	t_token	*token;

	if (cur && cur->type == type)
		return (cur);
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->quoted = type == T_DOUBLE_QUOTE || type == T_SINGLE_QUOTE;
	token->separator = type == T_SEPARATOR || type == T_PIPE;
	token->buffer = str_new();
	if (!token->buffer)
	{
		free_token(token);
		return (NULL);
	}
	if (!tokens)
		return (token);
	if (!lst_push(tokens, token))
	{
		free_token(token);
		return (NULL);
	}
	return (token);
}

int	tokenize(t_list *tokens, char **line, t_token *parent)
{
	t_token	*current;
	int		escaped;
	t_token	empty;
	char	c;

	empty = null_token();
	current = parent;
	escaped = parent->type == T_SINGLE_QUOTE;
	while (**line)
	{
		c = **line;
		++*line;
		if (c == '\\' && !escaped)
		{
			if (!(current->quoted))
				current = new_token(tokens, parent->type, current);
			escaped = 1;
			continue ;
		}
		else if (current->type != T_SINGLE_QUOTE && c == '$' && !escaped)
		{
		}
		if (!parent->quoted && !escaped)
		{
			if (c == '|')
				current = new_token(tokens, T_PIPE, current);
			else if (c == '\"')
				current = new_token(tokens, T_DOUBLE_QUOTE, current);
			else if (c == '\'')
				current = new_token(tokens, T_SINGLE_QUOTE, current);
			else if (c == ' ' || c == '\t')
				current = new_token(tokens, T_WHITESPACE, current);
			else if (c == '<')
				current = new_token(tokens, T_REDIRECT_IN, current);
			else if (c == '>')
				current = new_token(tokens, T_REDIRECT_OUT, current);
			else if (c == ';')
				current = new_token(tokens, T_SEPARATOR, current);
			else if (current->type != T_VAR)
				current = new_token(tokens, T_WORD, current);
		}
		else if ((c == '\"' && !escaped && parent->type == T_DOUBLE_QUOTE))
			return (TRUE);
		if (current->type == T_NONE)
			current = new_token(tokens, T_WORD, current);
		str_cappend(current->buffer, c);
		escaped = parent->type == T_SINGLE_QUOTE;
	}
	// if (escaped)
		// ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
	// return (!escaped && super_type == T_WORD);
	return TRUE;
}
