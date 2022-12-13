#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <mimemisc_priv.h>

struct attrib *
attrib_create(void)
{
	struct attrib *ret;

	ret = malloc(sizeof *ret);

	ret->a_n = 0;
	ret->a_alloc = 0;
	ret->a_name = NULL;
	ret->a_value = NULL;

	return ret;
}

void
attrib_add(struct attrib *a, const char *name, const char *value)
{
	attrib_addnodup(a, strdup(name), strdup(value));
}

void
attrib_addnodup(struct attrib *a, char *name, char *value)
{
	if (a->a_n + 1 >= a->a_alloc) {
		a->a_alloc = 2 * (a->a_alloc == 0 ? 4 : a->a_alloc);
		a->a_name = realloc(a->a_name, a->a_alloc * sizeof *a->a_name);
		a->a_value = realloc(a->a_value,
		    a->a_alloc * sizeof *a->a_value);
	}
	a->a_name[a->a_n] = name;
	a->a_value[a->a_n] = value;
	a->a_n++;
}

const char *
attrib_get(struct attrib *a, const char *name, int *cnt)
{
	int i;

	for (i = cnt == NULL ? 0 : *cnt; i < a->a_n; i++)
		if (strcasecmp(name, a->a_name[i]) == 0) {
			if (cnt != NULL)
				*cnt = i;
			return a->a_value[i];
		}

	return NULL;
}

void
attrib_free(struct attrib *a)
{
	int i;

	for (i = 0; i < a->a_n; i++) {
		free(a->a_name[i]);
		free(a->a_value[i]);
	}

	free(a->a_name);
	free(a->a_value);
	free(a);
}

char *
bd_makenorm(const char *bd, const char *crlfpair)
{
	char *ret;

	ret = malloc(strlen(bd) + strlen(crlfpair) * 2 + 3);

	strcpy(ret, crlfpair);
	strcat(ret, "--");
	strcat(ret, bd);
	strcat(ret, crlfpair);

	return ret;
}

char *
bd_maketail(const char *bd, const char *crlfpair)
{
	char *ret;

	ret = malloc(strlen(bd) + strlen(crlfpair) * 2 + 5);

	strcpy(ret, crlfpair);
	strcat(ret, "--");
	strcat(ret, bd);
	strcat(ret, "--");
	strcat(ret, crlfpair);

	return ret;
}

static char *
getquotedstring(const char *qstr, const char **end)
{
	int n;
	char *spc;
	int len;
	int alloc;

	while(*qstr && isspace(*qstr)) qstr++;

	if (*qstr != '"') {
		n = strspn(qstr, "!#$%&'*+-=.0123456789?ABCDEFGHIJKLMNOPQRSTUVWXYZ^_`abcdefghijklmnopqrstuvwxyz{|}~");
		spc = malloc(sizeof *spc * (n + 1));
		strncpy(spc, qstr, n);
		spc[n] = '\0';

		*end = qstr + n;
		
		return spc;
	}

	spc = malloc((alloc = 64));
	len = 0;
	spc[0] = '\0';

	qstr++;

	for (;;) {
		if (*qstr == '\\') {
			spc[len++] = *++qstr;
			qstr++;
		} else if (*qstr == '\n') {
			spc[len++] = *++qstr;
			qstr++;
		} else if (*qstr == '"')
			break;
		else
			spc[len++] = *qstr++;
		if (len >= alloc)
			spc = realloc(spc, (alloc *= 2));
	}

	spc[len] = '\0';

	*end = ++qstr;

	return spc;
}

static int
addparameter(struct attrib *a, const char **ctr)
{
	const char *ct;
	char *eq;
	char *name;
	char *value;
	char *endname;

	ct = *ctr;

	if ((ct = strchr(ct, ';')) == NULL)
		return 0;

	ct++;
	while (isspace(*ct))
		ct++;

	if ((eq = strchr(ct, '=')) == NULL)
		return 0;

	for(endname = eq; endname > ct; --endname)
		if(!isspace(endname[-1])) break;

	name = malloc(sizeof *name * (endname - ct + 1));
	strncpy(name, ct, endname - ct);
	name[endname - ct] = '\0';

	ct = eq + 1;
	value = getquotedstring(ct, &ct);

	attrib_addnodup(a, name, value);

	*ctr = ct;

	return 1;
}

struct attrib *
mime_getattrib(const char *ct)
{
	struct attrib *ret;

	ret = attrib_create();

	while (addparameter(ret, &ct));

	return ret;
}
