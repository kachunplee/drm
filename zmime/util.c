#include <string.h>

char *
memmem(const char *big, int blen, const char *lit, int llen)
{
	const char *pos;

	for (pos = big; pos - big <= blen - llen; pos++) {
		if (*pos == *lit && memcmp(pos, lit, llen) == 0)
			return (char *)pos;
	}

	return NULL;
}
