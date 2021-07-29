#include "minishell.h"

static void	push_argument(t_list **lst, t_command *command, char **arg,
	int *params)
{
	t_redirect	*redirection;
	int			fd;
	int			append;

	fd = params[0];
	append = params[1];
	if (*lst == command->redirect_in || *lst == command->redirect_out)
	{
		redirection = malloc(sizeof(t_redirect));
		if (redirection)
			*redirection = (t_redirect){fd, *arg, append};
		lst_push(*lst, redirection);
		params[0] = 1;
		params[1] = FALSE;
	}
	else
		lst_push(*lst, *arg);
	*arg = NULL;
	*lst = command->args;
}

int	parse(t_command *command)
{
	char		*argument;
	t_token		*cur;
	t_iterator	it;
	int			space;
	t_list		*lst;
	int			*redirect_params;

	space = 0;
	lst = command->args;
	it = iterator_new(command->children);
	argument = NULL;
	redirect_params = (int [2]){1, FALSE};
	while (iterator_has_next(&it))
	{
		cur = (t_token *)iterator_next(&it);
		if (cur->type == T_WHITESPACE)
		{
			space = 1;
			continue ;
		}
		if (cur->type == T_NUMBER && ((t_token *)it.current->next->value)
			&& ((t_token *)it.current->next->value)->type == T_REDIRECT_OUT)
		{
			redirect_params[0] = ft_atoi(*cur->buffer);
			continue ;
		}
		if ((cur->separator || is_redirection(NULL, cur) || space) && argument)
			push_argument(&lst, command, &argument, redirect_params);
		if (cur->type == T_REDIRECT_OUT)
		{
			lst = command->redirect_out;
			redirect_params[1] = ft_strlen(*cur->buffer) == 2;
		}
		else if (cur->type == T_REDIRECT_IN)
			lst = command->redirect_in;
		else if (!cur->separator)
			parse_token(cur, &argument);
		space = 0;
	}
	if (argument)
		push_argument(&lst, command, &argument, redirect_params);
	return (TRUE);
}
