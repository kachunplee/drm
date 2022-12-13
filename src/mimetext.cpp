#include "def.h"
#include "dmsg.h"
#include "mimetext.h"

MimeText::MimeText ()
{
	m_buf = NULL;
	m_szbuf = 0;
	m_charset = NULL;
	m_szcharset = 0;
}

MimeText::~MimeText ()
{
	delete m_buf;
	delete m_charset;
}

extern int base64_decode(const char *in, int len, char *out, int /* not use */);
extern int quoted_decode(const char *in, int len, char *out, int crlfsize, bool subj);

const char *
MimeText::GetText (const char * p)
{
	if (m_charset)
	{
		delete m_charset;
		m_charset = NULL;
		m_szcharset = 0;
	}

	//
	// If any encode string..
	//
	const char * q = strstr(p, "=?");
	if (q == NULL)
		return p;

	//
	// Allocate buffer...
	//
	int slen = strlen(p);
	if (m_szbuf < (slen+1))
	{
		delete m_buf;
		m_szbuf = slen+1;
		m_buf = new char [m_szbuf];
	}

	char * d;
	for(d = m_buf;q; q = strstr(p, "=?"))
	{
		// copy regular substg
		int n = q-p;
		if (n)
		{
			strncpy(d, p, n);
			d += n;
		}

		// decode
		p = DecodeText(q, d);
	}

	// copy the last piece
	if (p)
	{
		strcpy(d, p);
		d[strlen(p)] = 0;
	}
	else
		*d = 0;
	return m_buf;
}

//
// Return end of encode text
//
const char *
MimeText::DecodeText(const char * p, char * & o)
{
	// Check for format: =?charset?enc?stg=?=

	// Recheck - just for save
	if ((p[0] != '=') && (p[1] != '?'))
	{
		DMSG(0, "MimeText::DecodeText: not encoded string %s", p);
		return p;
	}

	const char * charset = p+2;
	const char * pEnc = strchr(charset, '?');
	if (pEnc == NULL)
	{
		// No charset -> Bad formated encode string
		strncpy(o, p, 2);
		o += 2;
		return p+2;
	}

	// Save charset
	if (m_szcharset < (pEnc-charset+1))
	{
		delete m_charset;
		m_szcharset = pEnc - charset + 1;
		m_charset = new char [m_szcharset];
	}
	strncpy(m_charset, charset, m_szcharset-1);
	m_charset[m_szcharset-1] = 0;

	if ((++pEnc)[1] != '?')
	{
		// No encode type -> Bad formated encode string
		strncpy(o, p, 2);
		o += 2;
		return p+2;
	}

	char enc;
	switch(pEnc[0])
	{
	case 'Q':
	case 'q':
		enc = 'Q';
		break;
	case 'B':
	case 'b':
		enc = 'B';
		break;
	default:
		// Unknown encode type -> Bad formated encode string
		strncpy(o, p, 2);
		o += 2;
		return p+2;
	}

	// Finally, ready to decode stg
	const char * stg = pEnc+2;
	const char * end = strstr(stg, "?=");
	if (end == NULL)
	{
		// No encoded end - just copy the '=?' over
		//	and let the main loop deal with the rest
		strncpy(o, p, 2);
		o += 2;
		return p+2;
	}

	int slen = end - stg;

	DMSG(0, "slen=%d stg=%s", slen, stg);

	int dlen = 0;
	switch(enc)
	{
	case 'Q':
		dlen = quoted_decode(stg, slen, o, 1, true);
		break;

	case 'B':
		dlen = base64_decode(stg, slen, o, 1);
		break;
	}

	if (dlen > 0)
	{
		o[dlen] = 0;
		DMSG(0, "dlen=%d stg=%s", dlen, stg);
		o += dlen;
	}


	return end+2;
}

MimeText gMimeText;
