#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <mimemisc.h>
#include <util.h>

#include <mime_priv.h>

const char *
mime_getvalue(struct mime_header *head, const char *key)
{
/*
	return mime_getvaluec(head, key, NULL);
*/

        const char * pValue = mime_getvaluec(head, key, NULL);
	if(pValue != NULL)
        	while(pValue && isspace(*pValue)) pValue++;
        return pValue;
}

const char *
mime_getvaluec(struct mime_header *head, const char *key, int *plc)
{
	return attrib_get((struct attrib *)head, key, plc);
}

static struct mime_header *
getinitmh()
{
	return (struct mime_header *)attrib_create();
}

static char *
foldvalue(char *value, int len, char **endptr, const char *crlfpair)
{
	char *end;
	char *ret;
	char *crlf;
	int alc;

	end = value + len;
	ret = NULL;
	alc = 0;

	while (value < end) {
		crlf = memmem(value, end - value, crlfpair, strlen(crlfpair));
		if (crlf == NULL)
			goto fv_error;

		ret = realloc(ret, crlf - value + alc + 1);
		memcpy(ret + alc, value, crlf - value);
		ret[crlf - value + alc] = '\0';
		alc += crlf - value;

		if (crlf[strlen(crlfpair)] == ' '
		    || crlf[strlen(crlfpair)] == '\t')
			value = crlf + strlen(crlfpair) + 1;
		else
			break;
	}

	*endptr = crlf + strlen(crlfpair);

	return ret;

fv_error:
	free(ret);
	return NULL;
}

static void
destroymh(struct mime_header *mh)
{
	attrib_free((struct attrib *)mh);
}

struct mime_header *
mime_parseheader(char *header, int len, char **last, const char *crlfpair)
{
	char *end;
	char *colon;
	char *crlf;
	char *name;
	char *value;
	struct attrib *ret;

	ret = (struct attrib *)getinitmh();
	end = header + len;

	while (header < end) {
		if (memcmp(header, crlfpair, strlen(crlfpair)) == 0) {
			header += strlen(crlfpair);
			break;
		}

		/*
		 * assume that we allways start at the begining of a
		 * header line
		 */
		colon = memchr(header, ':', end - header);
		crlf = memmem(header, end - header, crlfpair, strlen(crlfpair));

		if (colon == NULL || crlf == NULL || colon > crlf)
		{
			if(crlf)
			{
				/* skip to end of line */
				header = crlf + strlen(crlfpair);
				continue;
			}

			goto mph_error;
		}

		/* get the key */
		name = malloc(colon - header + 1);
		memcpy(name, header, colon - header);
		name[colon - header] = '\0';

		value = foldvalue(colon + 1, end - colon - 1, &header,
		    crlfpair);

		attrib_addnodup(ret, name, value);
	}

	if (last != NULL)
		*last = header;
		
	return (struct mime_header *)ret;

mph_error:
	destroymh((struct mime_header *)ret);
	return NULL;
}

void
mime_headerdelete(struct mime_header *mh)
{
	destroymh(mh);
}
