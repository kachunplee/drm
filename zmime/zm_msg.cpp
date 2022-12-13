#include <ctype.h>
#include <stdlib.h>
#include <string.h>

extern "C"
{
#include <mime_priv.h>
#include <ct.h>
#include <util.h>
}

#include "zmime.h"

void
ZMIME::GetMessage (char *msg, int len, const char *crlfpair)
{
	const char *bd;
	char *bdcm;
	int bdcmlen;
	char *pos;
	char *bdlc;
	const char *ct;
	struct attrib *a;

	if ((mm_headers = mime_parseheader(msg, len, &pos, crlfpair)) == NULL)
		return;

	ct = mime_getvalue(mm_headers, "content-type");

	if (ct == NULL || ct_cmptype(ct, "multipart") != 0)
	{
		/* not a multipart of no content-type */
	notmime:
		m_bodylen = len - (pos - msg);
		m_body = new char [m_bodylen];
		memcpy(m_body, pos, m_bodylen);

		const char * endbody = m_body + m_bodylen;
		char * f;
		for(char * p = m_body; p < endbody;)
		{
			if(strncmp(p, "begin ", 6) == 0)
			{
				p += 6;
				while(p < endbody && *p == ' ') p++;
				for(f = p; f < endbody && isdigit(*f); f++) ;
				if(*f == ' ' && f > p)
				{
					++f;
					for(p = f; p < endbody &&
							*p != '\n'; p++);
					if(p > f)
					{
						m_UUFile = string(f, p-f);
						break;
					}
				}
			}

			// skip to next line
			while(p < endbody) if(*p++ == '\n') break;
		}
	}
	else
	{
		/* multipart message */
		a = mime_getattrib(ct);
		bd = attrib_get(a, "boundary", NULL);
		if(bd == NULL || bd[0] == 0) goto notmime;
		/*
		 * XXX - need to be fixed to support "\r\n"
		 */
		bdcm = bd_makenorm(bd, crlfpair);
		bdcmlen = strlen(bdcm);
		if(strncmp(pos, bdcm+1, bdcmlen-1) == 0)
			bdlc = pos;
		else
		{
			bdlc = memmem(pos, len - (pos - msg), bdcm, bdcmlen);
			if (bdlc == NULL)
				goto notmime;

			if (bdlc != pos) {
				m_prologlen = bdlc - pos;
				m_prolog = new char [m_prologlen];
				memcpy(m_prolog, pos, m_prologlen);
			}
			bdlc++;
		}

		free(bdcm);

		ParseMultipart(bdlc, len - (bdlc - msg), bd, &pos, crlfpair);

		if (pos - msg < len) {
			m_epiloglen = len - (pos - msg);
			m_epilog = new char [m_epiloglen];
			memcpy(m_epilog, msg, m_epiloglen);
		}

	}
}

void
ZMIME::ParseMultipart (char *msg, int len, const char *bd, 
    char **endpos, const char *crlfpair)
{
	char *next;
	char *bdtail;
	char *bdnorm;
	int bdnormlen;
	int bdtaillen;
	int bbdtail;

	bdnorm = bd_makenorm(bd, crlfpair);
	bdnormlen = strlen(bdnorm);
	bdtail = bd_maketail(bd, crlfpair);
	bdtaillen = strlen(bdtail);

	char * bdnormdash = strstr(bdnorm, "--");
	int n = strlen(bdnormdash);
	if (memcmp(msg, bdnormdash, n) == 0)
	{
		msg += n;
		len -= n;

		bbdtail = 0;
		for (;;) {
			if ((next = memmem(msg, len, bdnorm, bdnormlen)) == NULL)
			{
				next = memmem(msg, len, bdtail, bdtaillen);
				if(next)
					bbdtail = 1;
			}

			if(next == NULL)
			{
				msg += len;
				break;
			}

			m_attachments.push_back(new ZMIME (msg, next - msg));

			len -= next - msg;
			msg = next;

			if (bbdtail)
			{
				msg += bdtaillen;
				break;
			}

			msg += bdnormlen;
			len -= bdnormlen;
		}
	}

	if (endpos != NULL)
		*endpos = msg;

	free(bdnorm);
	free(bdtail);
}

//-------------------- Decode ------------------------------------

static int
hexdigit (char a)
{
	if (a >= '0' && a <= '9')
		return a - '0';
	if (a >= 'a' && a <= 'f')
		return a - 'a' + 10;
	if (a >= 'A' && a <= 'F')
		return a - 'A' + 10;

	abort();
}

int
quoted_decode (const char *in, int len, char *out, int crlfsize, bool subj)
{
	int inpos;
	int cnt;

	cnt = 0;

	for (inpos = 0; inpos < len; ) {
		char c = in[inpos++];
		if (c == '=') {
			/* special handling */
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
			*out++ = ((subj && (c == '_')) ? ' ' : c);
			cnt++;
		}
	}

	return cnt;
}

static int
quoted_body_decode (const char *in, int len, char *out, int crlfsize)
{
	return quoted_decode(in, len, out, crlfsize, false);
}


int
base64_decode (const char *in, int len, char *out, int /* not use */)
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
	{ "quoted-printable", 1, quoted_body_decode },
	{ "base64", (float)3 / 4, base64_decode }
};

int
ZMIME::EstimateRawSize ()
{
	const char *cte;
	char *t;
	unsigned len;
	int i;

	if (m_bodylen <= 0)
		return 0;

	if ((cte = mime_getvalue(mm_headers, "content-transfer-encoding"))
	    == NULL)
		return m_bodylen;

	while (isspace(*cte))
		cte++;

	if ((t = strchr(cte, ';')) == NULL)
		len = strlen(cte);
	else
		len = t - cte;

	for (i = 0; i < (int) (sizeof no_encode / sizeof *no_encode); i++)
		if (len == strlen(no_encode[i]) && strncasecmp(cte,
		    no_encode[i], len) == 0)
			return m_bodylen;

	for (i = 0; i < (int) (sizeof encode / sizeof *encode); i++)
		if (len == strlen(encode[i].name) && strncasecmp(cte,
		    encode[i].name, len) == 0)
			return (int) (m_bodylen * encode[i].multiple);

	return -1;
}

int
ZMIME::GetRawBody (char * out)
{
	const char *cte;
	char *t;
	unsigned len;
	int i;

	if (m_bodylen <= 0)
		return 0;

	if ((cte = mime_getvalue(mm_headers, "content-transfer-encoding"))
	    == NULL) {
		memcpy(out, m_body, m_bodylen);
		return m_bodylen;
	}

	while (isspace(*cte))
		cte++;

	if ((t = strchr(cte, ';')) == NULL)
		len = strlen(cte);
	else
		len = t - cte;


	for (i = 0; i < (int) (sizeof encode / sizeof *encode); i++)
		if (len == strlen(encode[i].name) && strncasecmp(cte,
		    encode[i].name, len) == 0)
			return encode[i].decode(m_body, m_bodylen, out, 1);

	return -1;
}
