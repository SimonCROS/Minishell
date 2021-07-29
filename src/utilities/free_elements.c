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

void	free_redirect(t_redirect *redirect)
{
	if (redirect)
		free(redirect->str);
	free(redirect);
}
