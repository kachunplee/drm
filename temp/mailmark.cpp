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

bool MarkMails (PCGI pCGI, const char * pUser,
	const char * pBoxName, bool bMarkRead)
{
	ZString	boxFile;
	if (strcasecmp(pBoxName, "in") == 0)
	{
		boxFile = DIR_MAIL;
		boxFile += pUser;
	}
	else
	{
		boxFile = DIR_LOCAL;
		boxFile += pUser;
		struct stat sb;
		if(stat(boxFile, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				// no local mailbox dir
				cerr << "mailmark error on " << pUser << '/'
					<< pBoxName << endl;
				return false;
			}
		}
		boxFile += "/mail";
		if(stat(boxFile, &sb) == -1)
		{
			if(errno == ENOENT)
			{
				// no local mailbox dir
				cerr << "mailmark error on " << pUser << '/'
					<< pBoxName << endl;
				return false;
			}
		}
		boxFile += "/";
		boxFile += pBoxName;
	}

	int nIdx;

	map<int, string> mails;
/*
	mails[24] = "<NIHZUUUJLACICHVIGQBLOXFTV@terra1.com.br>";
	char *p;
*/
	char *p, *q, *r;
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

		//cerr << "mailmark " << nIdx << ' ' << s << endl;
	}

	Zifstream bFile(boxFile, ios::nocreate|ios::in);
	if(!bFile || !bFile.good())
	{
		// box file is bad, do nothing
		cerr << "mailmark open error " << boxFile << endl;
		return false;
	}

	ZString tmpFile = boxFile + ".tmp";
	int fd;
	if((fd = open(tmpFile, O_RDWR|O_CREAT|O_EXCL, 0660)) == -1)
	{
		cerr << "mailmark fail to create " << tmpFile << endl;
		return false;
	}

	// scan through headers
	map<int,string>::iterator iter;
	map<int,string>::iterator mails_end = mails.end();
	int nScan = 0;
	nIdx = 0;
	iter = mails.begin();
	bool bMatchIdx = false;
	bool bMatchMsgID = false;
	bool bHeader = false;
	const char * msgID;
	do
	{
		if (bFile.eof())
			break;

		p = bFile.GetLine();
		if(!p)
			break;		// end of box, done

		if (bHeader && bMatchIdx)
		{
		    if(strncasecmp(p, "Message-Id: ", 12) == 0)
		    {
			if (strncmp(p+12, msgID, strlen(msgID)) == 0)
			{
				// cerr << "find mark msgID " << msgID << endl;
				bMatchMsgID = true;
			}
		    }
		    else if(strncmp(p, "Status: ", 8) == 0)
		    {
			if (bMatchIdx && bMatchMsgID)
			{
				if (bMarkRead)
				{
					write(fd, p, strlen(p));
					if (strchr(p+8, 'O') == NULL)
						write(fd, "O", 1);
					if (strchr(p+8, 'R') == NULL)
						write(fd, "R", 1);
					write(fd, "\n", 1);
					bMatchIdx = false;
					bMatchMsgID = false;
				}
				continue;
			}
		    }
		    else if (!*p)
		    {
			if (bMatchIdx && bMatchMsgID && bMarkRead)
			{
				bMatchIdx = false;
				bMatchMsgID = false;
				write(fd, "Status: OR\n", 11);
			}
			bHeader = false;
			write(fd, "\n", 1);
			continue;
		    }
		}
		else
		{
		    if(strncmp(p, "From ", 5) == 0)
		    {
			// beginning of a new message
			if(++nScan > nIdx)
			{
				if (iter == mails_end)
				{
					// done marking, copy rest of mailbox
					write(fd, p, strlen(p));
					write(fd, "\n", 1);
					break;
				}
				
				nIdx = (*iter).first;
				msgID = (*iter).second.c_str();
				iter++;
			}
			bHeader = true;
			bMatchMsgID = false;
			bMatchIdx = false;
			if (nScan == nIdx)
			{
				bMatchIdx = true;
			}
		    }
		}
		write(fd, p, strlen(p));
		write(fd, "\n", 1);
	} while(bFile.NextLine());

	// done marking, copy rest of mailbox
	while(bFile.NextLine())
	{
		if (bFile.eof())
			break;
		p = bFile.GetLine();
		if(!p)
			break;
		if (*p)
			write(fd, p, strlen(p));
		write(fd, "\n", 1);
	}

	close(fd);
	chown(tmpFile, UserID, MailGID);
	chmod(tmpFile, 0660);
	ZString oldName = boxFile + ".o";
	rename(boxFile, oldName); 
	rename(tmpFile, boxFile);
/*
	remove(boxFile.o);
*/
	return true;
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

/*
char * pAction = "r";
char * pMboxName = "in";
*/
	char * pMboxName = cgi_GetValueByName(pCGI, "mboxname");
	if(pMboxName == NULL || !ValidateName(pMboxName))
	{
		cerr << pUser << ": mailmark: No mboxname" << endl;
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}
	char * pAction = cgi_GetValueByName(pCGI, "actiontype");
	if(pAction == NULL)
	{
		cerr << pUser << ": mailmark: No pAction" << endl;
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}

	if(*pAction != 'r' && *pAction != 'u')
	{
		cerr << pUser << ": mailmark: bad " << pAction << endl;
		OutErrorMsg("<h2>Input format error...</h2>");
		return(-1);
	}
	bool bMarkRead = true;
	if (*pAction == 'u')
		bMarkRead = false;

	//
	// Mark the mail messages
	//
	MarkMails(pCGI, stgUser, pMboxName, bMarkRead);

	if (bXML)
	{
		cout << "Content-Type: text/plain" << endl << endl
			<< pMboxName << " mark "
			<< (*pAction == 'r' ? "read" : "unread") << endl;
		return(0);
	}

	cout << "Location: " << (bHTTPS?"https:":"http:");
	char * host = getenv("HTTP_HOST");
	if (host) cout << "//" << host;
	cout << MAILBIN << "/folder?" << pMboxName << "+";
	cout << tp.tv_sec << endl << endl;
	return(0);
}
