#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *string_table[10];
#define table_len (sizeof string_table / sizeof string_table[0])

/* Examples:
 * create 8 foo bar
 * create 0 x
 */
static int create_string(char *p)
{
	unsigned int index;
	unsigned int offset;
	int ret = sscanf(p, "%u %n", &index, &offset);

	if (ret != 1 || index >= table_len)
		return -1;
	p += offset;

	char *new_str = realloc(string_table[index], strlen(p) + 1);
	if (!new_str)
		return -1;
	strcpy(new_str, p);
	string_table[index] = new_str;
	return 0;
}

/* delete 3
 * delete 4
 */
static int delete_string(const char *p)
{
	unsigned int index;
	int ret = sscanf(p, "%u", &index);
	if (ret != 1 || index >= table_len)
		return -1;
	free(string_table[index]);
	string_table[index] = 0;
	return 0;
}

/* combine 0 1 2
 * This concatenates strings in index 1 and 2 and stores it at 0.
 * Strings 1 and 2 are deleted afterward.
 */
static int combine_strings(const char *p)
{
	unsigned int a, b, c;
	int ret = sscanf(p, "%u %u %u", &a, &b, &c);

	if (ret != 3 || a >= table_len || b >= table_len || c >= table_len)
		return -1;
	if (!string_table[b] || !string_table[c])
		return -1;

	size_t len = strlen(string_table[b]) + strlen(string_table[c]);
	char *new_str = realloc(string_table[b], len+1);
	if (!new_str)
		return -1;
	strcat(new_str, string_table[c]);

	/* Since we realloced string_table[b], we should not free it.
	 * We do need to free string_table[a] and string_table[c]. */
	if (a != b && a != c)
		free(string_table[a]);
	string_table[a] = new_str;
	if (a != c && b != c)
	{
		free(string_table[c]);
		string_table[c] = 0;
	}
	return 0;
}

static int print_string(char *p)
{
	unsigned int index;
	int ret = sscanf(p, "%u", &index);
	if (ret != 1 || index >= table_len)
		return -1;
	if (string_table[index] == 0)
		return -1;
	puts(string_table[index]);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		fprintf(stderr, "Usage: %s COMMAND_FILE\n", argv[0]);
		return 1;
	}

	FILE *fp = argc == 2? fopen(argv[1], "r"):stdin;
	if (!fp)
	{
		perror(argv[0]);
		return 1;
	}
	void *p = malloc(172);

	/* Read commands line at a time and operate on the string table. */
	char *line = 0;
	size_t capacity = 0;
	ssize_t length;
	int ret = 0;
	while ((length = getline(&line, &capacity, fp)) > 0)
	{
		/* Remove newline */
		if (line[length - 1] == '\n')
			line[length - 1] = 0;
		char *p = line;
		const char *command = strsep(&p, " ");
		if (!command)
			ret = -1;
		else if (strcmp(command, "create") == 0)
			ret = create_string(p);
		else if (strcmp(command, "delete") == 0)
			ret = delete_string(p);
		else if (strcmp(command, "combine") == 0)
			ret = combine_strings(p);
		else if (strcmp(command, "print") == 0)
			ret = print_string(p);
		else
			ret = -1;
		if (ret)
		{
			fprintf(stderr, "Error with line: %s", line);
			break;
		}
	}
	if (argc == 2)
		fclose(fp);

	free(p);
	return ret == 0? 0:1;
}
