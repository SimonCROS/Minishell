#include "minishell.h"

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
