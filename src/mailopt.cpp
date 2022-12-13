#include <sys/types.h>
#include <sys/stat.h>

#include "def.h"
#include "mailopt.h"

char * strtohex (char * p, char * s)
{
	char c;
	char * q = s;
	unsigned int i;
	for (i = 0; i < strlen(p); i++)
	{
		c = (p[i] >> 4) & 0xf;
		if (c > 9)
			*q++ = c - 10 + 'A';
		else
			*q++ = c + '0';
		c = p[i] & 0xf;
		if (c > 9)
			*q++ = c - 10 + 'A';
		else
			*q++ = c + '0';
	}
	*q = 0;
	return s;
}

char * hextostr (char * p, char * s)
{
	char * q = s;
	unsigned int nlen = strlen(p);
	for (unsigned int i = 0; i < nlen;)
	{
		*q++ = (hex(*p)<<4) + hex(*(p+1));
		p += 2;
		i += 2;
	}
	*q = 0;
	return s;
}

MailOption::MailOption (int ac, char ** av, const char * pProg,
	const char * pUser)
{
	Init(pProg, pUser);

	if(ac == 0)
	{
		if(av && av[0])
			PrxPathInfo(av[0]);
		return;
	}

	if(ac == 1)
		return;

	while(ac > 1 && *(av[1]) == '-')
	{
		// setup special options
// later...
		SetCmd(av[1]+1);
		av++;
		ac--;
	}

	if(--ac <= 1)
		return;

	SetOpt(ac, ++av);
}

void
MailOption::Init (const char * pProg, const char * pUser)
{
	m_ProgName = pProg;
	m_ToAddr = "";
	m_Command = CMD_DSPLIST;
	m_cStatus = 'O';
	m_nMailIdx = 0;
	m_stgMailID = "";
	m_bXML = FALSE;
	m_bPreview = FALSE;
	m_bHeader = FALSE;
	m_bPrnMsg = FALSE;
	m_bFolderFrame = TRUE;
	m_bPreviewFrame = TRUE;
	m_ListFontSize = "xx-small";
	m_TZName = "PST8DST";
	m_DateFormat = "";
	m_SortOpt = SORT_DEF;

	m_POPAccount = 0;

	m_pAttNum = NULL;
	m_pFileName = NULL;

	GetPOPConfig(pUser);
}

void
MailOption::PrxPathInfo (char * info)
{
	if(info == NULL)
		return;

	if(*info == '/')
		info++;

	if(*info == 0)
		return;

	int ac = 0;
	char * av[3];
	char * q;

	while(info && !isdigit(*info))
	{
		q = strchr(info, '/');
		if(q) *q = 0;
		if(*info == '-')
			SetCmd(info+1);
		if(q == NULL) return;
		info = q + 1;
	}

	while(ac < 3)
	{
		q = strchr(info, '/');
		if(q) *q = 0;
		av[ac++] = info;
		if(q == NULL) break;	
		info = q + 1;
	}

	SetOpt(ac, av);
}

void
MailOption::SetCmd(char * pOpt)
{
	char * p;
	switch(*pOpt)
	{
	case M_XML:
		m_bXML = TRUE;
		break;

	case M_PREVIEW:
		m_bPreview = TRUE;
		break;

	case M_PRNMSG:
		m_bPrnMsg = TRUE;
		break;

	case M_BODYFRM:
		m_bHeader = FALSE;
		break;

	case M_HDRFRM:
		m_bHeader = TRUE;
		break;

	case M_DELETE:
		m_Command = CMD_DELETE;
		break;

	case M_MAIL:		// sendmail
	case M_REPLY:		// reply
	case M_REPLYALL:	// reply all
	case M_FORWARD:		// forward
		if(*pOpt == M_REPLY || *pOpt == M_REPLYALL)
			m_cStatus = 'P';
		else if(*pOpt == M_FORWARD)
			m_cStatus = 'F';
		
		m_nMailType = *pOpt++;
		m_ToAddr = "";
		for (p = pOpt; *p; p++)
		{
			m_ToAddr += *p;
			if (*p == ',') m_ToAddr += " ";
		}
		m_Command = CMD_SEND;
		break;

	case M_XFER:
		m_Command = CMD_XFER;
		m_XferTo = pOpt+1;
		break;

	case M_HEADER:
		m_Command = CMD_DSPHDR;
		break;

	case M_APPROVE:
		m_Command = CMD_APPROVE;
		break;

	case M_POP:
		m_POPAccount = atoi(pOpt+1);
		break;

	case M_SORT:
		switch(pOpt[1])
		{
		case 'd':
			m_SortOpt = SORT_DATE_A;
			break;
		case 'e':
			m_SortOpt = SORT_DATE_R;
			break;
		case 's':
			m_SortOpt = SORT_SUBJ_A;
			break;
		case 't':
			m_SortOpt = SORT_SUBJ_R;
			break;
		case 'w':
			m_SortOpt = SORT_WHO_A;
			break;
		case 'x':
			m_SortOpt = SORT_WHO_R;
			break;
		}
	}
}

void
MailOption::SetOpt(int ac, char **av)
{
	if(m_Command == CMD_DSPLIST)
		m_Command = CMD_DSPMSG;

	if(m_cStatus == 'O')
		m_cStatus = 'R';

	//
	// Use arguments
	//
	m_nMailIdx = atoi(av[0]);
	char * q = strchr(av[0], '.');
	if(q)
		m_pAttNum = q+1;

	if(--ac > 0)
	{
		// strip '\\'
		strstrip(av[1], '\\');
		char * p = av[1];
		char s[strlen(p)*2+1];
		if (*p == '<')
		{
			m_stgMailID = p;
			m_stgHexMailID = strtohex(p, s);
		}
		else
		{
			m_stgHexMailID = p;
			m_stgMailID = hextostr(p, s);
		}

/*
		char s[strlen(p)+1];
		char * q = s;
		while(*p)
		{
			if(*p ==  '%')
			{
				if(*(p+1) && *(p+2))
				{
					// convert pair of hex char to integer
					*q++ = (hex(*(p+1))<<4) + hex(*(p+2));
					p += 3;
					continue;
				}
			}
			*q++ = *p++;
		}
		*q = '\0';

		m_stgMailID = s;
*/
	}

	if(--ac > 0)
		m_pFileName = av[2];
}

void
MailOption::GetPOPConfig (const char * pUser)
{
	// read the POP configuration from the option file
	ZString stgTmp = DIR_LOCAL;
	stgTmp += pUser;
	stgTmp += "/.fromrc";
	struct stat sb;
	if(stat(stgTmp, &sb) == -1)
		return;

	Zifstream rcFile(stgTmp, ios::nocreate|ios::in);
	if(!rcFile || !rcFile.good())
		return;

	char * p;
	char * pName, * pServers, *pAuths;
	int i;
	do
	{
		pName = pServers = pAuths = NULL;
		p = rcFile.GetLine();
		if(!p) break;
		if(strncmp(p, "pop", 3) == 0)
		{
			p = p+3;
			i = atoi(p);
			if(i > 0 && i <= MAXPOPS)
			{
				pName = strchr(p, '\t');	
				if(pName)
				{
					*pName++ = 0;
					pServers = strchr(pName, '\t');	
					if(pServers)
					{
						*pServers++ = 0;
						pAuths = strchr(pServers, '\t');
						if(pAuths) *pAuths++ = 0;
					}
				}

				if(pName && pServers && pAuths)
				{
					m_POPs[--i].popName = pName;
					m_POPs[i].popServers = pServers;
					m_POPs[i].popAuths = pAuths;
				}
			}
		}
		else if(strncmp(p, "folderframe", 11) == 0)
		{
			p = strchr(p, '\t');	
			if (p)
			{
				p++;
				if (strncmp(p, "off", 3) == 0)
					m_bFolderFrame = FALSE;
			}
		}
		else if(strncmp(p, "previewframe", 12) == 0)
		{
			p = strchr(p, '\t');	
			if (p)
			{
				p++;
				if (strncmp(p, "off", 3) == 0)
					m_bPreviewFrame = FALSE;
			}
		}
		else if(strncmp(p, "listfontsize", 12) == 0)
		{
			m_ListFontSize = p+13;
		}
		else if(strncmp(p, "tzname", 6) == 0)
		{
			m_TZName = p+7;
		}
		else if(strncmp(p, "dateformat", 10) == 0)
		{
			m_DateFormat = p+11;
		}
	} while(rcFile.NextLine());
}

BOOL
MailOption::IsPOP (int pop)
{
	pop--;
	return (m_POPs[pop].popName.length() > 0 &&
		m_POPs[pop].popServers.length() > 0 &&
		m_POPs[pop].popAuths.length() > 0);
}
