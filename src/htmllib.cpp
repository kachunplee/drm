#include <string.h>
#include "def.h"

static bool gType = false;
ostream& HTMLBeg (ostream &stm, int type)
{
	gType = type;
	switch (type)
	{
	case 1:
		stm << "Content-type: text/plain" << endl << endl;
		return stm;
	case 2:
		stm << "Content-type: text/xml" << endl << endl;
		stm << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
			"<data>" << endl;
		return stm;
	}

	stm << "Content-type: text/html" << endl << endl;
	stm << "<html>" << endl;
	return stm;
}

ostream& HTMLEnd (ostream &stm)
{
	switch (gType)
	{
	case 1:
		break;
	case 2:
		stm << "</data>" << endl;
		break;
	default:
		stm << "</body></html>" << endl;
	}
	return stm;
}

ostream&
OutJSStg (ostream & stm, const char * p, bool bQuote)
{
	const char * spc = "<'\\" ;

	int n;
	char c;

	if (bQuote) stm << '\'';

	for(;;)
	{
		n = strcspn(p, spc);
		stm.write(p, n);
		switch((c = p[n]))
		{
		case 0:
			break;

		case '<':
			// mainly to break up </script>
			stm << "<' + '";
			break;

		case '\'':
		case '\\':
			stm << '\\' << c;
			break;
		}

		if (c == 0)
			break;
		p += n+1;
	}

	if (bQuote) stm << '\'';

	return stm;
}

ostream&
OutAmp (ostream &stm, bool bJS)
{
	stm << (bJS ? "&amp;" : "&");
	return stm;
}

ostream&
OutHTMLStg (ostream & stm, const char * p, bool bJS)
{
	const char * spc = bJS ? "&<>\"\033'\\" : "&<>\"\033" ;
	const char * q;
	int n;
	char c;
	for(;;)
	{
		n = strcspn(p, spc);
		stm.write(p, n);

		switch((c = p[n]))
		{
		case 0:
			return stm;

		case '\033':
			stm << c;
			p += n+1;
			if(strncmp(p, "\044B", 2) != 0)
				continue;

			if((q = strstr(p, "\033(")))
			{
				if(*(q+2) && (*(q+2) == 'B' || *(q+2) == 'J'))
				{
					if (bJS)
					{
						char * pTmp = new char [q-p+1];
						strncpy(pTmp, p, q-p);
						pTmp[q-p] = 0;
						OutHTMLStg(stm, pTmp, bJS);
						delete [] pTmp;
					}
					else
						stm.write(p, q-p);
					stm.write(q, 3);
					p = q + 3;
				}
			}
			continue;

		case '"':
			stm << "&quot;";
			break;	

		case '\'':
		case '\\':
			stm << '\\' << c;
			break;

		case '&':
			//OutAmp(stm, bJS) << "amp;";
			stm << "&amp;";
			break;

		case '<':
			//OutAmp(stm, bJS) << "lt;";
			stm << "&lt;";
			break;

		case '>':
			//OutAmp(stm, bJS) << "gt;";
			stm << "&gt;";
			break;
		}

		p += n+1;
	}

	return stm;
}

//
//
ostream& operator << (ostream& stm, const HTMLText& txt)
{
	const char * p = txt.m_stg;
	if(p == NULL || *p == '\0')
		return stm;

	return OutHTMLStg(stm, p, txt.m_bJS);
}

HTMLText::HTMLText (const char * p, int n, bool bJS)
{
	m_pText = NULL;
	m_alloc = new char [n+1];
	strncpy(m_alloc, p, n);
	m_alloc[n] = 0;
	m_stg = m_alloc;
	m_bJS = bJS;
}

const char * HTMLText::GetText ()
{
	if(m_pText)
		delete m_pText;

	int nLimit = strlen(m_stg);
	m_pText = new char[nLimit*6+1];
	if(m_pText == NULL)
		return NULL;

	char * p = (char *)m_stg;
	if(p == NULL || *p == '\0')
		return NULL;

	char * q = m_pText;
	char * r;
	for(int n = 0; n < nLimit && *p; n++, p++)
	{
		switch(*p)
		{
		case '\033':
			*q++ = *p;
			if(strncmp(p+1, "\044B", 2) != 0)
				break;

			if((r = strstr(p+1, "\033(")))
			{
				if(*(r+2) && (*(r+2) == 'B' || *(r+2) == 'J'))
				{
					strncpy(q, p+1, r+3-(p+1));
					q += (r+3-(p+1));
				}
			}
			break;
		case '&':
			strcpy(q, "&amp;");
			q += 5;
			break;
		case '<':
			strcpy(q, "&lt;");
			q += 4;
			break;
		case '>':
			strcpy(q, "&gt;");
			q += 4;
			break;
		case '"':
			strcpy(q, "&quot;");
			q += 6;
			break;
		default:
			*q++ = *p;
		}
	}
	*q = 0;
	return m_pText;
}
