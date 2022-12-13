#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <ct.h>

int
ct_cmpct(const char *ct, const char *fct)
{
	char *semicolon;

	while (isspace(*ct))
		ct++;

	if ((semicolon = strchr(ct, ';')) == NULL)
		semicolon = strchr(ct, '\x0');

	while (isspace(*(semicolon - 1)))
		semicolon--;

	if (strlen(fct) != semicolon - ct)
		return -1;

	return strncasecmp(ct, fct, semicolon - ct);
}

int
ct_cmptype(const char *ct, const char *type)
{
	char *slash;

	while (isspace(*ct))
		ct++;

	if ((slash = strchr(ct, '/')) == NULL)
		return -1;

	if (strlen(type) != slash - ct)
		return 1;

	return strncasecmp(ct, type, slash - ct);
}

int
ct_cmpsubtype(const char *ct, const char *subtype)
{
	char *end;

	if ((ct = strchr(ct, '/')) == NULL)
		return -1;

	ct++;

	if ((end = strchr(ct, ';')) == NULL) {
		end = strchr(ct, '\0');
		while (isspace(*(end - 1)))
			end--;
	}

	if (strlen(subtype) != end - ct)
		return 1;

	return strncasecmp(ct, subtype, end - ct);
}
