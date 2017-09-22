#include <cstdio>
#include <cstring>
#include <cstdlib>

class SubStringReference
{
	const char *start;
	size_t len;

public:
	SubStringReference(const char *s, size_t l) : start(s), len(l) { }
	virtual ~SubStringReference() { }
	virtual const char *getStart() const { return start; }
	virtual int getLen() const { return len; }
};

void print_sub_string(const SubStringReference& str)
{
	char buf[68];
	if (str.getLen() >= sizeof buf)
	{
		// Only copy sizeof(buf) - 1 bytes plus a null
		memcpy(buf, str.getStart(), sizeof(buf) - 1);
		buf[sizeof(buf) - 1] = '\0'; // null-terminate
	}
	else
	{
		// The length is less than the size of buf so just string copy.
		strcpy(buf, str.getStart());
		buf[str.getLen()] = '\0'; // null-terminate to get just the substring
	}
	puts(buf);
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s STRING START LENGTH\n", argv[0]);
		return 1;
	}
	const char *s = argv[1];
	int total_len = strlen(s);
	int start = atoi(argv[2]);
	int len = atoi(argv[3]);

	if (start < 0 || start >= total_len)
	{
		fputs("start is out of range!\n", stderr);
		return 1;
	}
	if (len < 0 || start + len > total_len)
	{
		fputs("length is out of range!\n", stderr);
		return 1;
	}
	SubStringReference str(s + start, len);
	print_sub_string(str);
	return 0;
}

