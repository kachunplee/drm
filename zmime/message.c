#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <mime_priv.h>
#include <ct.h>
#include <util.h>

static int
hexdigit(char a)
{
	if (a >= '0' && a <= '9')
		return a - '0';
	if (a >= 'a' && a <= 'f')
		return a - 'a' + 10;
	if (a >= 'A' && a <= 'F')
		return a - 'A' + 10;

	abort();
}

static int
quoted_decode(char *in, int len, char *out, int crlfsize)
{
	int inpos;
	int cnt;

	cnt = 0;

	for (inpos = 0; inpos < len; ) {
		if (in[inpos] == '=') {
			/* special handling */
			inpos++;
			if ((in[inpos] >= '0' && in[inpos] <= '9')
			    || (in[inpos] >= 'A' && in[inpos] <= 'F')
			    || (in[inpos] >= 'a' && in[inpos] <= 'f')) {
				/* encoding a special char */
				*out++ = hexdigit(in[inpos]) << 4
				    | hexdigit(in[inpos + 1]);
				inpos += 2;
				cnt++;
			} else
				inpos += crlfsize;	/* crlf size */
		} else {
			*out++ = in[inpos++];
			cnt++;
		}
	}

	return cnt;
}

static int
base64_decode(char *in, int len, char *out, int crlfsize)
{
	int cnt;
	int inpos;
	int i;
	int bits;
	int eqcnt;

	cnt = 0;
	inpos = 0;
	eqcnt = 0;
	while (inpos < len && eqcnt == 0) {
		bits = 0;
		for (i = 0; inpos < len && i < 4; inpos++) {
			switch (in[inpos]) {
			case 'A':  case 'B':  case 'C':  case 'D':  case 'E':
			case 'F':  case 'G':  case 'H':  case 'I':  case 'J':
			case 'K':  case 'L':  case 'M':  case 'N':  case 'O':
			case 'P':  case 'Q':  case 'R':  case 'S':  case 'T':
			case 'U':  case 'V':  case 'W':  case 'X':  case 'Y':
			case 'Z':
				bits = (bits << 6) | (in[inpos] - 'A');
				i++;
				break;
			case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
			case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
			case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
			case 'p':  case 'q':  case 'r':  case 's':  case 't':
			case 'u':  case 'v':  case 'w':  case 'x':  case 'y':
			case 'z':
				bits = (bits << 6) | (in[inpos] - 'a' + 26);
				i++;
				break;
			case '0':  case '1':  case '2':  case '3':  case '4':
			case '5':  case '6':  case '7':  case '8':  case '9':
				bits = (bits << 6) | (in[inpos] - '0' + 52);
				i++;
				break;
			case '+':
				bits = (bits << 6) | 62;
				i++;
				break;
			case '/':
				bits = (bits << 6) | 63;
				i++;
				break;
			case '=':
				bits <<= 6;
				i++;
				eqcnt++;
				break;
			default:
			}
		}

		if (i == 0 && inpos >= len)
			continue;

		switch (eqcnt) {
		case 0:
			*out++ = (bits >> 16) & 0xff;
			*out++ = (bits >> 8) & 0xff;
			*out++ = bits & 0xff;
			cnt += 3;
			break;
		case 1:
			*out++ = (bits >> 16) & 0xff;
			*out++ = (bits >> 8) & 0xff;
			cnt += 2;
			break;
		case 2:
			*out++ = (bits >> 16) & 0xff;
			cnt += 1;
			break;
		}
	}

	return cnt;
}

static char *no_encode[] = {
	"7bit", "8bit", "binary"
};

static struct {
	char *name;
	float multiple;
	int (*decode)(char *, int, char *, int);
} encode[] = {
	{ "quoted-printable", 1, quoted_decode },
	{ "base64", (float)3 / 4, base64_decode }
};

int
mime_estimaterawsize(struct mime_header * headers, int bodylen)
{
	const char *cte;
	char *t;
	int len;
	int i;

	if (bodylen <= 0)
		return 0;

	if ((cte = mime_getvalue(headers, "content-transfer-encoding"))
	    == NULL)
		return bodylen;

	while (isspace(*cte))
		cte++;

	if ((t = strchr(cte, ';')) == NULL)
		len = strlen(cte);
	else
		len = t - cte;

	for (i = 0; i < sizeof no_encode / sizeof *no_encode; i++)
		if (len == strlen(no_encode[i]) && strncasecmp(cte,
		    no_encode[i], len) == 0)
			return bodylen;

	for (i = 0; i < sizeof encode / sizeof *encode; i++)
		if (len == strlen(encode[i].name) && strncasecmp(cte,
		    encode[i].name, len) == 0)
			return bodylen * encode[i].multiple;

	return -1;
}

int
mime_getrawbody(struct mime_header *headers, char * body, int bodylen, char *out, int crlfsize)
{
	const char *cte;
	char *t;
	int len;
	int i;

	if (bodylen <= 0)
		return 0;

	if ((cte = mime_getvalue(headers, "content-transfer-encoding"))
	    == NULL) {
		memcpy(out, body, bodylen);
		return bodylen;
	}

	while (isspace(*cte))
		cte++;

	if ((t = strchr(cte, ';')) == NULL)
		len = strlen(cte);
	else
		len = t - cte;


	for (i = 0; i < sizeof encode / sizeof *encode; i++)
		if (len == strlen(encode[i].name) && strncasecmp(cte,
		    encode[i].name, len) == 0)
			return encode[i].decode(body, bodylen,
			    out, crlfsize);

	return -1;
}
