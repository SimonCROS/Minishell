#include "minishell.h"

#include <stdio.h>

void	free_token(t_token *token)
{
	if (token)
	{
		if (token->buffer)
			free(*(token->buffer));
		free(token->buffer);
		lst_destroy(token->children);
	}
	free(token);
}

t_token	null_token(void)
{
	return ((t_token){});
}

static t_token	*new_token(t_list *tokens, t_token_type type, t_token *cur)
{
	t_token	*token;

	if (cur && cur->type == type && cur->type != T_VAR)
		return (cur);
	token = malloc(sizeof(t_token));
	if (!token)
		return (NULL);
	token->type = type;
	token->quoted = type == T_DOUBLE_QUOTE || type == T_SINGLE_QUOTE;
	token->separator = type == T_SEPARATOR || type == T_PIPE;
	token->buffer = str_new();
	token->children = lst_new((t_con)free_token);
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

int	is_valid_variable_char(char c, char *str)
{
	if (ft_isdigit(str[0]))
		return (FALSE);
	if (str[0] == '?')
		return (FALSE);
	if (!str[0])
		return (ft_isalnum(c) || c == '_' || c == '?');
	else
		return (ft_isalnum(c) || c == '_');
}

int	tokenize(t_token *parent, char **line)
{
	t_token	*current;
	int		escaped;
	t_token	empty;
	char	c;

	empty = null_token();
	current = &empty;
	escaped = parent->type == T_SINGLE_QUOTE;
	while (**line)
	{
		c = **line;
		++*line;
		if (c == '\\' && !escaped)
		{
			if (!(current->quoted))
				current = new_token(parent->children, T_WORD, current);
			escaped = 1;
			continue ;
		}
		else if (current->type != T_SINGLE_QUOTE && c == '$' && !escaped)
		{
			current = new_token(parent->children, T_VAR, current);
			continue ;
		}
		if (!parent->quoted && !escaped)
		{
			if (c == '|')
				current = new_token(parent->children, T_PIPE, current);
			else if (c == '\"')
			{
				current = new_token(parent->children, T_DOUBLE_QUOTE, current);
				if (!tokenize(current, line))
					return (FALSE);
				continue ;
			}
			else if (c == '\'')
			{
				current = new_token(parent->children, T_SINGLE_QUOTE, current);
				if (!tokenize(current, line))
					return (FALSE);
				continue ;
			}
			else if (c == ' ' || c == '\t')
				current = new_token(parent->children, T_WHITESPACE, current);
			else if (c == '<')
				current = new_token(parent->children, T_REDIRECT_IN, current);
			else if (c == '>')
				current = new_token(parent->children, T_REDIRECT_OUT, current);
			else if (c == ';')
				current = new_token(parent->children, T_SEPARATOR, current);
			else if (current->type != T_VAR
				|| !is_valid_variable_char(c, *current->buffer))
				current = new_token(parent->children, T_WORD, current);
		}
		else if ((c == '\"' && !escaped && parent->type == T_DOUBLE_QUOTE))
			return (TRUE);
		else if ((c == '\'' && parent->type == T_SINGLE_QUOTE))
			return (TRUE);
		else if (current->type != T_VAR
			|| !is_valid_variable_char(c, *current->buffer))
			current = new_token(parent->children, T_WORD, current);
		str_cappend(current->buffer, c);
		escaped = parent->type == T_SINGLE_QUOTE;
	}
	if (escaped || parent->quoted)
		ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
	return (!parent->quoted);
}
