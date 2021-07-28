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

t_token	*new_token(t_token *parent, t_token_type type, t_token *cur, int p)
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
	token->parent = parent;
	if (!token->buffer || !token->children
		|| (p && !lst_push(parent->children, token)))
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
	t_token	*cur;
	int		escaped;
	t_token	empty;
	char	c;

	empty = null_token();
	cur = &empty;
	escaped = parent->type == T_SINGLE_QUOTE;
	while (**line)
	{
		c = **line;
		++*line;
		if (c == '\\' && !escaped)
		{
			if (!(cur->quoted))
				cur = new_token(parent, T_WORD, cur, TRUE);
			escaped = 1;
			continue ;
		}
		else if (cur->type != T_SINGLE_QUOTE && c == '$' && !escaped)
		{
			cur = new_token(parent, T_VAR, cur, TRUE);
			continue ;
		}
		if (!parent->quoted && !escaped)
		{
			if (c == '|')
				cur = new_token(parent, T_PIPE, cur, TRUE);
			else if (c == '\"')
			{
				cur = new_token(parent, T_DOUBLE_QUOTE, cur, TRUE);
				if (!tokenize(cur, line))
					return (FALSE);
				continue ;
			}
			else if (c == '\'')
			{
				cur = new_token(parent, T_SINGLE_QUOTE, cur, TRUE);
				if (!tokenize(cur, line))
					return (FALSE);
				continue ;
			}
			else if (c == ' ' || c == '\t')
				cur = new_token(parent, T_WHITESPACE, cur, TRUE);
			else if (ft_isdigit(c)
				&& (cur->type == T_NUMBER || cur->type == T_WHITESPACE
					|| cur->type == T_NONE || cur->separator))
				cur = new_token(parent, T_NUMBER, cur, TRUE);
			else if (c == '<')
				cur = new_token(parent, T_REDIRECT_IN, cur, TRUE);
			else if (c == '>')
				cur = new_token(parent, T_REDIRECT_OUT, cur, TRUE);
			else if (c == ';')
				cur = new_token(parent, T_SEPARATOR, cur, TRUE);
			else if (cur->type != T_VAR
				|| !is_valid_variable_char(c, *cur->buffer))
				cur = new_token(parent, T_WORD, cur, TRUE);
		}
		else if ((c == '\"' && !escaped && parent->type == T_DOUBLE_QUOTE))
			return (TRUE);
		else if ((c == '\'' && parent->type == T_SINGLE_QUOTE))
			return (TRUE);
		else if (cur->type != T_VAR
			|| !is_valid_variable_char(c, *cur->buffer))
			cur = new_token(parent, T_WORD, cur, TRUE);
		str_cappend(cur->buffer, c);
		escaped = parent->type == T_SINGLE_QUOTE;
	}
	if (escaped || parent->quoted)
		ft_putendl_fd("minish: syntax error: unexpected end of file", 2);
	return (!parent->quoted);
}
