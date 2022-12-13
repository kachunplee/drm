#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <string>

#include "uudecode.h"

extern "C"
{
#include "ct.h"
#include "mime_priv.h"
}

#include "zmime.h"

ZMIME::ZMIME (const char * msg, int len)
{
	m_contentType = NULL;
	m_contentIDs = NULL;

	m_bDecoded = false;

	mm_headers = NULL;

	m_body = NULL;
	m_bodylen = 0;
	m_prolog = NULL;
	m_prologlen = 0;
	m_epilog = NULL;
	m_epiloglen = 0;
	m_cid = false;

	GetMessage((char *)msg, len, "\n");
}

ZMIME::~ZMIME ()
{
	if (mm_headers) mime_headerdelete(mm_headers);

	delete [] m_body;
	delete [] m_prolog;
	delete [] m_epilog;

	int n = m_attachments.size();
	for (int i = 0; i < n; i++)
		delete m_attachments[i];
	delete m_contentIDs;
}

const char *
ZMIME::GetHeader (const char * h)
{
	return mm_headers ? mime_getvalue(mm_headers, h) : NULL;
}

const char nullhd [] = "";

const char *
ZMIME::GetHeaderC (const char * h)
{
	const char * p = GetHeader(h);
	return p ? p : nullhd;
}

void
ZMIME::DecodeBody ()
{
	if(!m_bDecoded)
	{
		if(m_bodylen)
		{
			char * buf = new char [m_bodylen];
			int n  = GetRawBody(buf);
			if(n < 0)
				delete [] buf;
			else
			{
				m_bodylen = n;
				delete [] m_body;
				m_body = buf;
			}
		}

		// Check for uuencode
		// Decode if encoded
		if(IsUUEncoded())
		{
		    const char *p, *f;
		    const char *endbody = m_body + m_bodylen;
		    for(p = m_body; p < endbody;)
		    {
			if(strncmp(p, "begin ", 6) == 0)
			{
				const char * uubegin = p;
				p += 6;
				while(p < endbody && *p == ' ') p++;
				for(f = p; f < endbody && isdigit(*f); f++) ;
				if(*f == ' ' && f != p)
				{
					++f;
					for(p = f; p < endbody &&
							*p != '\n'; p++);
					m_UUFile = string(f, p-f);
					++p;

					char * buf = (char *)
						new char [(endbody-p+3)*3/4];
					UUDecode uu(buf);

					const char * q;
					while(p < endbody)
					{
						if(strncmp(p, "end", 3) == 0)
						{
							p += 3;
							while(p < endbody &&
							   *p++ != '\n') ;
							break;
						}

						for(q = p; q < endbody &&
							  *q != '\n'; q++) ;

						if(q-p) uu.Decode(p, q-p);
						p = q+1;
					}

					m_bodylen = uu.GetBuf() - buf;

					if(uubegin > m_body)
					{
						delete m_prolog;
						m_prologlen = uubegin - m_body;
						m_prolog = new char [m_prologlen];
						memcpy(m_prolog, m_body, m_prologlen);
					}

					if(p < endbody)
					{
						delete m_epilog;
						m_epiloglen = endbody - p;
						m_epilog = new char [m_epiloglen];
						memcpy(m_epilog, p, m_epiloglen);
					}


					delete [] m_body;
					m_body = buf;
					break;
				}
			}
			// skip to next line
			while(p < endbody) if(*p++ == '\n') break;
		    }
		}

		m_bDecoded = true;
	}
}

ZMIME &
ZMIME::GetAtt (int i)
{
	return *m_attachments[i];
}

ZMIME *
ZMIME::GetAtt (const char * att)
{
	ZMIME * msg = this;
	int n;
	while(*att)
	{
		n = atoi(att);
		if(n < 1 || n > msg->GetNumAtt())
			break;

		msg = msg->m_attachments[n-1];

		att = strchr(att, '.');
		if(att++ == NULL) break;
	}

	return msg;
}

bool
ZMIME::HasAttachment ()
{
	switch(GetNumAtt())
	{
	case 0:
		return !IsPlain() && !IsHTML();

	case 1:
		return GetAtt(0).HasAttachment();
	}

	return !IsPlainAndHTML() ;
}

bool
ZMIME::IsPlainAndHTML ()
{
	return GetNumAtt() == 2 && GetAtt(0).IsPlain() && GetAtt(1).IsHTML();
}

bool
ZMIME::IsPlain ()
{
	return !IsUUEncoded() && ct_cmpsubtype(GetContentType(), "plain") == 0;
}

bool
ZMIME::IsHTML ()
{
	return ct_cmpsubtype(GetContentType(), "html") == 0;
}

bool
ZMIME::IsUUEncoded ()
{
	return (m_UUFile.length() > 0);
}

bool
ZMIME::IsCID ()
{
	return m_cid;
}

void
ZMIME::SetCID ()
{
	m_cid = true; 
}

const char *
ZMIME::GetAttFileName ()
{
	if(m_AttFile.length() == 0)
	{
		const char * p = GetHeader("Content-Disposition");
		if(p == NULL || strncmp(p, "attachment",  10) != 0)
			return NULL;

		struct attrib * ata = mime_getattrib(p);
		if(ata == NULL)
			return NULL;

		p = attrib_get(ata, "filename", NULL);
		if(p) m_AttFile = p;
		attrib_free(ata);
		if(p == NULL)
			return NULL;
	}

	return m_AttFile.c_str();
}

const char *
ZMIME::GetUUFileName ()
{
	return m_UUFile.c_str();
}

const char *
ZMIME::GetContentType ()
{
	if(m_contentType)
		return m_contentType;

	m_contentType = GetHeader("content-type");
	if(m_contentType == NULL) m_contentType = "text/plain";
	return m_contentType;
}


const char *
ZMIME::MapContentID (const char * id)
{
	if(m_contentIDs == NULL)
	{
		m_contentIDs = new map<string, string>;
		AddContentID(*m_contentIDs);
	}

	map<string, string>::iterator iter = m_contentIDs->find(id);
	if(iter == m_contentIDs->end())
		return NULL;

	return (*iter).second.c_str();
}

void
ZMIME::AddContentID(map<string, string> & ids, const char * pAttNumR)
{
	const char * p = GetHeader("content-id");
	string * s = NULL;
	if(p)
	{
		int n = strlen(p);
		if(*p == '<')
		{
			++p;
			--n;
			if(p[n-1] == '>')
				--n;
			s = new string(p, n);
			p = s->c_str();
		}

		ids[p] = pAttNumR;
		delete s;
	}

	ZMIME * att = NULL;
	int n = strlen(pAttNumR);
	char attnum[n+20];
	strcpy(attnum, pAttNumR);
	if(n) attnum[n++] = '.';
	
	for(int i = 0; i < GetNumAtt(); i++)
	{
		sprintf(attnum+n, "%d", i+1);
		att = m_attachments[i];
		att->AddContentID(ids, attnum);
	}
} 
