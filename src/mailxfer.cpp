#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#include <map>

#include "def.h"
#include "inbox.h"
#include "cgi.h"

#include "mailopt.h"

BOOL bLynx = FALSE;
uid_t UserID = 0; 
gid_t UserGID = 0; 
uid_t HttpID = 0; 
gid_t MailGID = 0; 

bool bXML = false;

void OutErrorMsg (char * p)
{
	cout << "Content-Type: text/html" << endl << endl
		<< DEF_BODY_TAG << endl;
	cout << HTMLText(p) << endl;
}

bool ValidateName (char * p)
{
	if(strcmp(p, "in") == 0)
		return true;
	if(strcmp(p, "out") == 0)
		return true;
	if(strcmp(p, "trash") == 0)
		return true;

	// Loop for folder
	//return FALSE;
	return true;
}

void TransferMails (PCGI pCGI, const char * pUser,
	const char * pSrcName, const char * pDestName)
{
	MailBox * pSrcBox;
	if(strcmp(pSrcName, "in") == 0)
		pSrcBox = new InBox(pUser);
	else
	{
		pSrcBox = new MailBox(pUser, pSrcName);
	}
	MailBox * pDestBox;
	if(strcmp(pDestName, "in") == 0)
		pDestBox = new InBox(pUser);
	else
	{
		pDestBox = new MailBox(pUser, pDestName);
	}
											cerr << pUser << ": xfer " << pDestName << " <- " << pSrcName << endl;

	int nIdx, nLen;
	struct MsgInfo * pInfo;
	caddr_t pMap;
	char *p, *q, *r;

	map<int, string> mails;
	for(int i=0; i<pCGI->EntryCount; i++)
	{
		p = pCGI->Entries[i].name;
		q = strchr(p, ',');
		if(!isdigit(*p) || q == NULL)
			continue;

		r = pCGI->Entries[i].val;
		if(r == NULL || strcmp(r, "yes") != 0)
			continue;

		nIdx = atoi(p);
		q++;
		p = q;
		char * s = q;
		if (*p != '<')
		{
			while(*p)
			{
				if(*(p) && *(p+1))
				{
					// convert pair of hex char to integer
					*q++ = (hex(*(p))<<4) + hex(*(p+1));
					p += 2;
					continue;
				}
			}
		}
		*q = '\0';

		mails[nIdx] = s;

		cerr << "got " << nIdx << ' ' << s << endl;
	}

	map<int,string>::iterator iter;
	map<int,string>::iterator mails_end = mails.end();
	Mails dels;
	const char * msgID;
	for (iter = mails.begin(); iter != mails_end; ++iter)
	{
		nIdx = (*iter).first;
		msgID = (*iter).second.c_str();

		// transfer to destination box
		if(pSrcBox->GetCurMailInfo(nIdx, msgID))
		{
			cerr << pUser << " xfer " << nIdx << ' ' << msgID << endl;
			pInfo = pSrcBox->GetInfo();
			pMap = pSrcBox->GetMsg(pInfo->msgOffset);
			nLen = pInfo->msgLength;
			dels[pMap] = nLen;
			if (!pDestBox->AppendMail(pMap, nLen))
			{
				cerr << "Error... Abort transfer" << endl;
				return;
			}
		}
	}


	pSrcBox->DeleteMail(dels);
}

int
main (int ac, char **av)
{
	bool bXferPrev = false;
	while (ac > 1)
	{
		if (strcmp(av[1], "-X") == 0)
		{
			--ac;
			++av;
			bXML = true;
		}
		else if (strcmp(av[1], "-p") == 0)
		{
			--ac;
			++av;
			bXferPrev = true;
		}
		else
			break;
	}

	char * pUser = getenv("REMOTE_USER");

	if(pUser == NULL || *pUser == '\0' || *pUser == '-')
	{
		cout << "Location: http://" << getenv("SERVER_NAME")
			<< "/accesserror.htm\n\n";
		return(1);
	}

	ZString stgUser = pUser;

	struct passwd * pPwdEnt = getpwnam(stgUser);
	if(pPwdEnt == NULL)
	{
		cout << "Location: http://" << getenv("SERVER_NAME")
			<< "/accesserror.htm\n\n";
		return(1);
	}
	UserID = pPwdEnt->pw_uid;
	UserGID = pPwdEnt->pw_gid;

	pPwdEnt = getpwnam("http");
	if(pPwdEnt == NULL)
		HttpID = UserID;
	else
		HttpID = pPwdEnt->pw_uid;

	struct group * pGroup = getgrnam("mail");
	if(pGroup == NULL)
		MailGID = UserGID;
	else
		MailGID = pGroup->gr_gid;

	BOOL bHTTPS = FALSE;
	char * pHTTPS = getenv("HTTPS");
	if(pHTTPS && strcmp(pHTTPS, "on") == 0)
		bHTTPS = TRUE;

cerr << "mailxfer for " << pUser << endl;

	PCGI pCGI = cgi_Create();
	if (!pCGI)
	{
		OutErrorMsg("<h2>The mail server is busy...</h2>\n<hr><h3>Please retry again later</h3>");
		return(1);
	}
	cgi_GetInput(pCGI);

	struct timeval tp;
	struct timezone tzp;
	gettimeofday(&tp, &tzp);

	char * pMboxName = cgi_GetValueByName(pCGI, "mboxname");
	if(pMboxName == NULL || !ValidateName(pMboxName))
	{
		cerr << pUser << ": mailxfer: No mboxname" << endl;
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}
	char * pDest = cgi_GetValueByName(pCGI, "actiontype");
	if(pDest == NULL)
	{
		cerr << pUser << ": mailxfer: No pDest" << endl;
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}

cerr << "pMboxName=" << pMboxName << ", pDest=" << pDest << endl;

	if(*pDest == 'x')
	{
		++pDest;
		if(!ValidateName(pDest))
		{
			cerr << pUser << ": mailxfer: bad " << pDest << endl;
			OutErrorMsg("<h2>Input format error...</h2>");
			return(-1);
		}
		//
		// Transfer the mail messages
		//
		if (strcmp(pMboxName, pDest) != 0)
			TransferMails(pCGI, stgUser, pMboxName, pDest);
	}
	else if(strcmp(pDest, "emptytrash") == 0)
	{
		//
		// Empty the trash box
		//
		MailBox xbox(stgUser, "trash");
		xbox.DeleteBox();
	}
	else if(strcmp(pDest, "emptyinbox") == 0)
	{
		//
		// Empty the in box
		//
		ZString boxfile = DIR_MAIL;
		boxfile += stgUser;
		unlink(boxfile);
		cerr << "drm: remove " << boxfile << endl;
		boxfile = DIR_MAIL;
		boxfile += ".";
		boxfile += stgUser;
		boxfile += ".pop";
		unlink(boxfile);
		cerr << "drm: remove " << boxfile << endl;
		boxfile = "/tmp/mail/.";
		boxfile += stgUser;
		boxfile += ".pop";
		unlink(boxfile);
		cerr << "drm: remove " << boxfile << endl;
	}
	else
	{
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}
	
	if (bXML)
	{
		cout << "Content-Type: text/plain" << endl << endl
			<< pMboxName << " -> " << pDest << endl;
		return(0);
	}

	cout << "Location: " << (bHTTPS?"https:":"http:");
	char * host = getenv("HTTP_HOST");
	if (host) cout << "//" << host;
	cout << MAILBIN;
	if (bXferPrev)
		cout << "frmlstpreview?-b+";
	else
		cout << "/folder?" << pMboxName << "+";
	cout << tp.tv_sec << endl << endl;
	return(0);
}
