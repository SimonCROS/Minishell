#include "minishell.h"

int	parse(t_command *command)
{
	char		*argument;
	t_token		*current;
	t_redirect	*redirection;
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
	while (iterator_has_next(&it))
	{
		current = (t_token *)iterator_next(&it);
		if (current->type == T_WHITESPACE)
		{
			space = 1;
			continue ;
		}
		if (current->type == T_NUMBER && ((t_token *)it.current->next->value)
			&& ((t_token *)it.current->next->value)->type == T_REDIRECT_OUT)
		{
			fd = ft_atoi(*current->buffer);
			continue ;
		}
		if ((current->separator || current->type == T_REDIRECT_IN || \
		current->type == T_REDIRECT_OUT || space) && argument)
		{
			if (lst == command->redirect_in || lst == command->redirect_out)
			{
				redirection = malloc(sizeof(t_redirect));
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
			redirection = malloc(sizeof(t_redirect));
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
