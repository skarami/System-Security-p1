#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PASSWORD_HASH "$5$afkj23234ASF$T.C3isXspLrawvd2smItz4/.r15VkfIW/xxIlgtVixB"

static int authorized;
static char log[1160];
static int log_size;

void log_attempt(int num_args, char *args[])
{
	/* Log the attempt. Check the size so we don't overflow the buffer! */
	log[0] = 0;
	log_size = 0;
	snprintf(log, sizeof log, "%s attempt to execute:",
		 authorized? "Authorized":"Unauthorized");
	log_size = strlen(log);
	for (int i = 0; i < num_args && strlen(args[i]) + log_size < sizeof log; ++i)
	{
		log[log_size] = ' ';
		log_size += 1 + strlen(args[i]);
		strcat(log, args[i]);
	}
}

void check_authorization(const char *pass)
{
	/* Do not actually use crypt in a real system! */
	const char *hash = crypt(pass, PASSWORD_HASH);
	authorized = strcmp(hash, PASSWORD_HASH) == 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		fprintf(stderr, "Usage: %s PASSWORD PATH [ARGUMENTS...]\n", argv[0]);
		return 1;
	}
	check_authorization(argv[1]);
	log_attempt(argc-2, argv+2);
	/* Sanity check the log size! */
	if (log_size >= (int)sizeof log)
		abort();
	fprintf(stderr, "%s\n", log);

	if (authorized)
	{
		/* The user is authorized to perform this action, so do it! */
		execvp(argv[2], &argv[2]);
		perror(argv[2]);
	}
	/* Either way, if we made it here, it's an error. */
	return 1;
}
