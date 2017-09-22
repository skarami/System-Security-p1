#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int foo(char *arg)
{
	char buf[284];
	strcpy(buf, arg);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s STRING\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	foo(argv[1]);
	return 0;
}
