#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define TARGET "/tmp/target6"

int main(void)
{
	char arg1[] = "password";
	char arg2[] = "echo";
	char arg3[] = "'hi!'";
	char *args[] = { TARGET, arg1, arg2, arg3, NULL };
	char *env[] = { NULL };

	if (0 > execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
