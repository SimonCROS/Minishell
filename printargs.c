#include <stdio.h>

int	main(int argc, char const *argv[])
{
	for (size_t i = 1; i < argc; i++)
		printf("\033[32m\"%s\"\033[0m\n", (char *)argv[i]);
}
