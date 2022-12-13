#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "def.h"
#include "mailopt.h"
#include "mailto.h"
#include "inbox.h"

BOOL bLynx = FALSE;
uid_t UserID = 0; 
gid_t UserGID = 0; 
uid_t HttpID = 0; 
gid_t MailGID = 0; 

int
main (int ac, char **av)
{
	bool bHeader = true;
	char * foldername = NULL;
	if (ac > 1)
	{
		if (av[1][0] == '-')
		{
			if (av[1][1] == 'h')
			{
				++av;
				--ac;
			}
			else if (av[1][1] == 'b')
			{
				bHeader = false;
				++av;
				--ac;
			}
		}
	}

	if (ac > 1)
	{
		++av;
		--ac;
		if (ac > 0)
		{
			foldername = av[0];
			if (*foldername == '/')
				++foldername;
		}
	}

	// sanity check... only alphanumeric?
	for(char * p = foldername; p && *p; p++)
	{
		if (isupper(*p)) *p = tolower(*p);
		else if (!islower(*p) && !isdigit(*p) && *p != '/')
		{
			//goto badmbox;
			cerr << "Illegal Mail Box " << foldername << endl;
			exit (1);
		}
	}

	string progname("/compose");
	if (foldername)
	{
		progname += "?";
		progname += foldername;
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
		HTMLBeg(cout) << DEF_BODY_TAG << endl
		     << "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
		     << "width=200 height=31>";
		cout << "<h3>The email account you submit is invalid</h3>"
			<< endl << "</body>" << endl << "</html>" << endl;
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

	char * path_info = getenv("PATH_INFO");
	MailOption options(path_info?0:ac,
				path_info?&path_info:av,
				progname.c_str(), pUser);

	MailBox * pbx = NULL;
	if (foldername)
		pbx = (strcmp(foldername, "in") == 0)
			? new InBox(stgUser)
			: new MailBox(stgUser, foldername);

	HTMLBeg(cout) << "<head>" << endl
		<< "<title>Send Mail - Direct Read "
		<< "Email - WebMail Interface</title>" << endl
	;

	struct MsgInfo * pInfo = NULL;
	caddr_t pMap = NULL;
	if(pbx && pbx->GetMailStatus() == MS_MAIL)
	{
		if(pbx->GetCurMailInfo(options.GetMailIdx(),
			options.GetMailID()))
		{
			pInfo = pbx->GetInfo();
			pMap = pbx->GetMsg(pInfo->msgOffset);
		}
	}
	MailTo mailto(&options, stgUser, pInfo, pMap, foldername, bHeader);
	cout << mailto;
	HTMLEnd(cout);

	delete pbx;
	return(0);
}
