#include <stdio.h>

int	main(int argc, char const *argv[])
{
	int	i;

	i = 0;
	while (i < argc)
		printf("\033[31m\"\033[32m%s\033[31m\"\033[0m\n", (char *)argv[i++]);
}
