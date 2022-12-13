#include <string.h>
#include "def.h"

ostream& operator << (ostream& stm, const URLText& txt)
{
	char * p = (char *)txt.m_stg;
	if(p == NULL || *p == '\0')
		return stm;
	char c;
	int n;
	for(;;)
	{
		n = strcspn(p, " \"%+");
		c = p[n];
		p[n] = 0;
		stm << p;
		switch(c)
		{
		case ' ':
			stm << "%20";
			break;
		case '"':
			stm << "%22";
			break;
		case '%':
			stm << "%25";
			break;
		case '+':
			stm << (txt.m_bScript ? "%252B" : "%2B");
			break;
		}
		if(c == 0)
			break;
		p[n] = c;
		p += n+1;
	}
	return stm;
}

URLText::URLText (const char * p, int n, bool bScript)
{
	m_alloc = new char [n+1];
	strncpy(m_alloc, p, n);
	m_alloc[n] = 0;
	m_stg = m_alloc;
	m_pText = NULL;
	m_bScript = bScript;
}

const char * URLText::GetText ()
{
	if(m_pText)
	{
		delete m_pText;
		m_pText = NULL;
	}

	int nLimit = strlen(m_stg);
	if(nLimit == 0)
		return NULL;

	m_pText = new char[nLimit*4+1];
	if(m_pText == NULL)
		return NULL;

	char * p = (char *)m_stg;
	if(p == NULL || *p == '\0')
		return NULL;

	char * q = m_pText;
	for(int n = 0; n < nLimit && *p; n++, p++)
	{
		switch(*p)
		{
		case ' ':
			*q++ = '%';
			*q++ = '2';
			*q++ = '0';
			break;
		case '"':
			*q++ = '%';
			*q++ = '2';
			*q++ = '2';
			break;
		case '%':
			*q++ = '%';
			*q++ = '2';
			*q++ = '5';
			break;
		case '+':
			if(m_bScript)
			{
				*q++ = '%';
				*q++ = '2';
				*q++ = '5';
				*q++ = '2';
				*q++ = '5';
			}
			else
				*q++ = '%';

			*q++ = '2';
			*q++ = 'B';
			break;
		default:
			*q++ = *p;
		}
	}
	*q = 0;
	return m_pText;
}

ostream& operator << (ostream& stm, const URLPostText& txt)
{
	char * p = (char *) txt.m_stg;
	if(p == NULL)
		return stm;
	char c;
	int n;
	for(;;)
	{
		n = strcspn(p, "\"%+@!");
		c = p[n];
		p[n] = 0;
		stm << p;
		switch(c)
		{
		case '"':
			stm << "%22";
			break;
		case '%':
			stm << "%25";
			break;
		case '+':
			stm << "%2B";
			break;
		case '@':
		case '!':
			stm << "_";
			break;
		}
		if(c == 0)
			break;
		p[n] = c;
		p += n+1;
	}
	return stm;
}
