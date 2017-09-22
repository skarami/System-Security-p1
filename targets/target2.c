#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct widget
{
	double x;
	double y;
	double z;
	int count;
};

#define MAX_WIDGETS 268

int foo(char *in, int count)
{
	struct widget buf[MAX_WIDGETS];

	if (count < MAX_WIDGETS)
		memcpy(buf, in, count * sizeof(struct widget));

	return 0;
}

void usage(const char *binary)
{
	fprintf(stderr, "Usage: %s COUNT,DATA\n", binary);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int count;
	char *in;

	if (argc != 2)
		usage(argv[0]);

	/*
	 * format of argv[1] is as follows:
	 *
	 * - a count, encoded as a decimal number in ASCII
	 * - a comma (",")
	 * - the remainder of the data, treated as an array
	 *   of struct widget
	 */

	count = (int)strtoul(argv[1], &in, 10);
	if (*in != ',')
		usage(argv[0]);
	in++;                         /* advance one byte, past the comma */
	foo(in, count);

	return 0;
}
