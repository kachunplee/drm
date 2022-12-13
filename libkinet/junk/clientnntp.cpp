#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <db.h>

#include <string>

#include "dmsg.h"
#include "cnntp.h"
#include "clientnntp.h"

#define CHKDB ((DB *) m_chkdb)

ClientNNTP::ClientNNTP (const char * servers, const char * auths,
    const char * chkf)
{
	m_servers = servers;
	m_auths = auths?auths:"";
	if (chkf) m_chkf = chkf;
	m_chkdb = NULL;
	m_pNNTP = NULL;

	m_headers = NULL;
	m_mHdr = 0;

	m_hdrBuf = NULL;
	m_hbSize = 0;

	m_offBuf = 0;

	m_oMsgID = m_oNewsgrps = m_oXref = -1;
	m_nArtSize = -1;
	m_nLines = -1;

	m_dataState = IN_CLOSE;
}

ClientNNTP::~ClientNNTP ()
{
	Close();
	CloseChkDB();
	delete m_headers;
	delete m_hdrBuf;
}

long
ClientNNTP::GetChkDB (const char * grp)
{
	if (m_chkf.c_str() == 0)
		return 0L;

	if (m_chkdb == NULL)
	{
		m_chkdb = dbopen(m_chkf.c_str(), O_RDONLY, 0644, DB_HASH, NULL);
		if (m_chkdb == NULL);
		{
		    	DMSG(0, "ClientNNTP: Cannot open %s: %s",
				m_chkf.c_str(), strerror(errno));
			m_chkf = "";
			return 0L;
		}
	}

	DBT key, data;
	key.data = (void *)grp;
	key.size = strlen(grp) + 1;
	if (CHKDB->get(CHKDB, &key, &data, 0) == 0)
		return *(long *)data.data;

	return 0L;
}

void
ClientNNTP::CloseChkDB ()
{
	if (m_chkdb)
		CHKDB->close(CHKDB);
}

bool
ClientNNTP::IsConnect ()
{
	if (m_pNNTP == NULL)
	{
	    m_pNNTP = new CNNTP(m_servers.c_str(), m_auths.c_str());
	}
	
	if ((m_pNNTP == NULL) || !m_pNNTP->IsConnected())
		return false;

	return true;
}

char *
ClientNNTP::command (const char * cmd, const char * av)
{
	FlushData();

	for(int retry = 0; retry < 5; ++retry)
	{
	    if (m_pNNTP == NULL)
	    {
		if (!IsConnect())	// Try connect
		{
		    sleep(10);
		    continue;
		}

		if ((strcasecmp(cmd, "GROUP") != 0) && (m_cGroup.length() > 0))
		{
		    DMSG(0, "ClientNNTP: Resend GROUP: %s", m_cGroup.c_str());
		    grpinfo gi;
		    if (!groupcmd(m_cGroup.c_str(), gi))
		    {
	    		DMSG(0, "ClientNNTP: %s resend group failed: %s %s: ... retrying %d",
				m_pNNTP->GetConnInfo(),
				cmd, (av?av:"(null)"), retry+1);
			Close();
			continue;
		    }

		    if (m_cArtnum > gi.min)
		    {
			char artnbuf[50];
			sprintf(artnbuf, "%u", m_cArtnum);
			DMSG(0, "ClientNNTP: Send STAT: %s", artnbuf);
			char *p = command("STAT", artnbuf); // set cArtNum
			if ((p == NULL) || (strncmp(p, "223 ", 4) != 0))
			{
	    			DMSG(0, "ClientNNTP: %s cArt %ld cannot set",
					m_pNNTP->GetConnInfo(), m_cArtnum);
				m_cArtnum = gi.min;
			}
		    }
		    else
			m_cArtnum = gi.min;
	        }
	    }

	    if (av != NULL)
	    {
		m_pNNTP->SetCmd("%s %s\r\n", cmd, av);
		DMSG(1, "=>%s %s", cmd, av);
	    }
	    else
	    {
		m_pNNTP->SetCmd("%s\r\n", cmd);
		DMSG(1, "=>%s", cmd);
	    }

	    char *  p = m_pNNTP->SendCmd();
	    if (p && (atoi(p) != 503))	// Server temporary not avaliable!
	    {
			DMSG(1, "<=%s %s", cmd, p);
			return p;
	    }

	    DMSG(0, "ClientNNTP: %s command %s %s: error: %s... retrying %d",
		m_pNNTP->GetConnInfo(), cmd, (av?av:"(null)"), (p?p:"(null)"), retry+1);
	    Close();
	    sleep(1);

	    m_pNNTP = NULL;
	}

	return NULL;
}

bool
ClientNNTP::groupcmd (const char * grp, grpinfo &gi)
{
	grpinfo gisave;
	gisave.numart = -1;
	CNNTP *pNNTP = NULL;

	for (int retry = 0; retry < 10; retry++)
	{
		gi.numart = -1;

		char *p = command("GROUP", grp);
		if ((p == NULL) || (strncmp(p, "211 ", 4) != 0))
		{
	        	DMSG(0, "ClientNNTP: %s groupcmd error: %s %s %s.. retrying %d",
				m_pNNTP->GetConnInfo(), grp, p);
			Close();
			retry += 2;	// shorter retry
			continue;
		}

		p += 4;
		while(isspace(*p)) p++;
		gi.numart = atoi(p);

		while(!isspace(*p)) p++;
		while(isspace(*p)) p++;
		gi.min = strtoul(p, NULL, 10);

		while(!isspace(*p)) p++;
		while(isspace(*p)) p++;
		gi.max = strtoul(p, NULL, 10);

		if (gi.numart > ((int)gi.max-(int)gi.min+1))
			gi.numart = gi.max-gi.min+1;

		long minnum = GetChkDB(grp);
		if (minnum == 0)
			break;

		if (gi.numart >= minnum)
			break;
		 
	        DMSG(0, "ClientNNTP: %s group retention: %s %ld > %ld... retrying %d",
			m_pNNTP->GetConnInfo(), grp, minnum, gi.numart);

		if ((gi.numart >= 0) && (gi.numart > gisave.numart))
		{
			Close(pNNTP);

			pNNTP = m_pNNTP;
			gisave = gi;
			m_pNNTP = NULL;
		}

		Close();
	}

	if (pNNTP && ((m_pNNTP == NULL) || (gisave.numart > gi.numart)))
	{
		Close();
		m_pNNTP = pNNTP;
		gi = gisave;
		pNNTP = NULL;
	}
	Close(pNNTP);
	return (m_pNNTP != NULL);
}

grpinfo
ClientNNTP::group (const char * grp)
{
	FlushData();

	static grpinfo gi;
	groupcmd(grp, gi);
	if (gi.numart < 0)
		return gi;

	m_cGroup = grp;
	m_cArtnum = gi.min;
	return gi;
}

char *
ClientNNTP::xover (unsigned beg, unsigned end, bool markin)
{
	char av[100];	// 2 integer with a dash - 100 should be plenty
	FlushData();

	if (end == 0)
	{
		sprintf(av, "%u", beg);
	}
	else
	{
		sprintf(av, "%u-%u", beg, end);
	}
	char * p =  command("XOVER", av);
	if ((p == NULL) || (strncmp(p, "224 ", 4) != 0))
		return NULL;

	if (markin) m_dataState = IN_DATA;
	return p;
}

char *
ClientNNTP::xhdr (const char * phdr, const char * prange, bool markin)
{
	char av[256];	// header type + range or msgid - 256 should be plenty
	FlushData();

	sprintf(av, "%s %s", phdr, prange);

	char * p =  command("XHDR", av);

	if (markin) m_dataState = IN_HEADER;
	return p;
}

char *
ClientNNTP::xpat (const char * phdr, const char * prange, const char * pattern,
	bool markin)
{
	char av[256];	// header type + range or msgid + pattern
			//	(512 should be enough)
	FlushData();

	sprintf(av, "%s %s %s", phdr, prange, pattern);

	char * p =  command("XPAT", av);

	if (markin) m_dataState = IN_HEADER;

	return p;
}


char *
ClientNNTP::listgroup ()
{
	FlushData();

	char *p = command("LISTGROUP");
	if ((p == NULL) || (strncmp(p, "211 ", 4) != 0))
		return NULL;

	m_dataState = IN_DATA;
	return p;
}

char *
ClientNNTP::article (const char * name, int t)
{
	FlushData();

	const char * cmd = ((t == 1) ? "HEAD" :
			       ((t == 2) ? "BODY" :"ARTICLE"));
	char *p = command(cmd, name);
	if ((p == NULL) || (atoi(p) != (220+t)))
		return NULL;

	m_name = name;
	m_artcmd = t;
	m_dataState = (t == 2) ? IN_DATA : IN_HEADER;

	m_iHdr = m_nHdr = 0;
	m_offBuf = 0;

	m_oMsgID = m_oNewsgrps = m_oXref = -1;
	m_nArtSize = -1;
	m_nLines = -1;

	if (name[0] != '<')
		m_cArtnum = strtoul(name+1, NULL, 10);

	return p;
}

char *
ClientNNTP::article (const char * name, bool hdronly)
{
	return article(name, hdronly?1:0);
}

bool
ClientNNTP::ArtRewind ()
{
	switch(m_dataState)
	{
	case IN_CLOSE:
		return false;

	case IN_HEADER:
		break;

	case IN_DATA:
		FlushData();
		return article(m_name.c_str(), m_artcmd);
	}

	m_iHdr = 0;
	return true;
}

char *
ClientNNTP::GetArtLine ()
{
	if (m_iHdr < m_nHdr)
		return m_hdrBuf + m_headers[m_iHdr++];

	switch(m_dataState)
	{
	case IN_CLOSE:
		return NULL;

	case IN_HEADER: {
		    char * p = GetHeader();
		    if (p) { m_iHdr++; return p; }
		}
		break;

	case IN_DATA:
		break;
	}

	return GetDataLine();
}

void
ClientNNTP::FlushData ()
{
	if (m_dataState != IN_CLOSE)
		while(GetDataLine()) /* */ ;
}

void
ClientNNTP::CloseArt ()
{
	FlushData();
}

int
ClientNNTP::GetArtSize()
{
	while (m_nArtSize < 0) {
		if (GetHeader() == NULL)
		{
			// Oop... no byte header

			// Get the header size
			m_nArtSize = m_offBuf;

			if (m_nLines > (int)m_nHdr) {
				// Just get a rough estimate of the body
				m_nArtSize += (m_nLines - m_nHdr)*40;
			}
			break;
		}
	}

	return m_nArtSize;
}

char *
ClientNNTP::GetArtMsgID ()
{
	while (m_oMsgID < 0) {
		if (GetHeader() == NULL)
			break;
	}
	return m_hdrBuf+m_oMsgID;
}

char *
ClientNNTP::GetArtNewsgrps ()
{
	while (m_oNewsgrps < 0) {
		if (GetHeader() == NULL)
			break;
	}
	return m_hdrBuf+m_oNewsgrps;
}


const char *
ClientNNTP::GetArtName ()
{
	if (m_name[0] != '<')
		return m_name.c_str();

	if (m_oXref >= 0)
		return m_hdrBuf + m_oXref;

	while (m_oXref < 0) {
		if (GetHeader() == NULL)
			return NULL;
	}

	char * p = m_hdrBuf + m_oXref;
	while(isspace(*p)) p++;         // skip spaces
	while(*p && !isspace(*p)) p++;  // skip system name
	while(isspace(*p)) p++;         // skip spaces
	char * q;
	for(q = p; *p && !isspace(*p); p++)
	{
		if(*p == ':') *p = '/';
	}

        *p = 0;
	return q;
}

//
//
const char szNewsgrps [] = "Newsgroups: ";
const char szMessageid [] = "Message-ID: ";
const char szXref [] = "Xref: ";
const char szByte [] = "Byte: ";
const char szLines [] = "Lines: ";

char *
ClientNNTP::GetHeader ()
{
	if (m_dataState == IN_DATA)
		return NULL;

        char *p = GetDataLine();
        if (p == NULL)
		return NULL;

        m_nLnLen = strlen(p);
        unsigned n = m_nLnLen+1;

	if (m_hdrBuf == NULL ||
		(m_offBuf+n) > m_hbSize)
	{
		char * oldbuf = m_hdrBuf;
		while((m_offBuf+n) > m_hbSize)
			m_hbSize += 4096;
		m_hdrBuf = new char [m_hbSize];
		if (oldbuf)
		{
			memcpy(m_hdrBuf, oldbuf, m_offBuf);
			delete(oldbuf);
		}

	}

	strcpy(m_hdrBuf+m_offBuf, p);
	unsigned off = m_offBuf;
	m_offBuf += n;
	if (m_headers == NULL || (m_nHdr >= m_mHdr))
	{
		unsigned * oldheader = m_headers;
		m_mHdr += 100;
		m_headers = new unsigned [m_mHdr];
		if (oldheader)
		{
			memcpy(m_headers, oldheader, m_nHdr*sizeof(unsigned));
			delete(oldheader);
		}
	}
	m_headers[m_nHdr++] = off;

	if (p[0] == 0)
	{
		m_dataState = IN_DATA;
		return p;
	}

	p = m_hdrBuf+off;
	if((m_oNewsgrps < 0)
            && (n > sizeof(szNewsgrps))
            && (strncasecmp(p, szNewsgrps, sizeof(szNewsgrps)-1) == 0))
		m_oNewsgrps = off + sizeof(szNewsgrps) - 1;

	if((m_oMsgID < 0)
            && (n > sizeof(szMessageid))
            && (strncasecmp(p, szMessageid, sizeof(szMessageid)-1) == 0))
                 m_oMsgID = off + sizeof(szMessageid) - 1;

	if((m_oXref < 0)
            && (n > sizeof(szXref))
            && (strncasecmp(p, szXref, sizeof(szXref)-1) == 0))
                 m_oXref = off + sizeof(szXref) - 1;

	if((m_nArtSize < 0)
            && (n > sizeof(szByte))
            && (strncasecmp(p, szByte, sizeof(szByte)-1) == 0))
	{
		
		char * q = p + sizeof(szByte)-1;
		while(isspace(*q)) q++;
		m_nArtSize = atoi(q);
	}

	if((m_nLines < 0)
            && (n > sizeof(szLines))
            && (strncasecmp(p, szLines, sizeof(szLines)-1) == 0))
	{
		
		char * q = p + sizeof(szLines)-1;
		while(isspace(*q)) q++;
		m_nLines = atoi(q);
	}

        return p;
}

char *
ClientNNTP::GetDataLine ()
{
	if (m_dataState == IN_CLOSE)
		return NULL;

	if(!IsConnect())
	{
	done:
		m_dataState = IN_CLOSE;
		return NULL;
	}

	char * p = m_pNNTP->GetLineRaw();

	if (p == NULL)
		goto done;

        if (p[0] == '.')
        {
                switch(p[1])
                {
                case '.':
                        p++;
			break;

                case 0:
			goto done;
                }
	}

        m_nLnLen = strlen(p);
	return p;
}

char *
ClientNNTP::GetLine (int * pn)
{
	if (IsConnect())
		return m_pNNTP->GetLine(pn);
	return NULL;
}

char *
ClientNNTP::GetLine ()
{
	if (IsConnect())
		return m_pNNTP->GetLine();

	return NULL;
}

bool
ClientNNTP::IsEOL ()
{
	if (IsConnect())
		return m_pNNTP->IsEOL();

	return false;
}

bool
ClientNNTP::Close (CNNTP * nntp)
{
	try
	{
		delete nntp;
	}
	catch (const char * p)
	{
		DMSG(0, "ClientNNTP::Close error %s", p);
		return false;
	}
	catch (int err)
	{
		DMSG(0, "ClientNNTP::Close error %s", strerror(err));
		return false;
	}
	catch (...)
	{
		DMSG(0, "ClientNNTP::Close unknown error");
		return false;
	}

	return true;
}

bool
ClientNNTP::Close ()
{
	CNNTP * nntp = m_pNNTP;
	m_pNNTP = NULL;
	return Close(nntp);
}
