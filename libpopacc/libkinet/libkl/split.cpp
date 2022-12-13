#include <string.h>
#include <ctype.h>
#include <vector>

/*
 * split char * by sep
 * warning: char * is changed by terminated nulls
 */
int 
klsplit(vector<char *> & v, char * p, const char * sep, int cnt)
{
	char * q;
	int l = sep ? strlen(sep) : 1;
	int i = 0;

	for(;;)
	{
		v.push_back(p);

		if ((++i >= cnt) && (cnt > 0))
			break;

		if (sep)
		{
			q = strstr(p, sep);
		}
		else
		{
			for(q = p; *q && !isspace(*q); q++) ;
			if (*q == 0) q = NULL;
		}

		if (q == NULL) break;

		*q = 0;
		if (sep)
		{
			p = q + l;
		}
		else
		{
			while(*++q && isspace(*q)) ;
			p = q;
		}
	}

	return i;
}
