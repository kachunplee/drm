#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <ndbm.h>
#include <pwd.h>
#include <grp.h>

#include "def.h"
#include "mailopt.h"
#include "mailto.h"
#include "maillhtml.h"
//#include "maillxml.h"
#include "mailldat.h"
#include "mailmsg.h"
#include "mailsettings.h"
#include "inbox.h"
#include "cpopacc.h"

BOOL bLynx = FALSE;
uid_t UserID = 0; 
gid_t UserGID = 0; 
uid_t HttpID = 0; 
gid_t MailGID = 0; 

extern int debug;

const int MAX_POP = 3;

static void
ErrorExit (const char * servers, const char * p)
{
	cout << "<h3>" << p
		<< " (" << servers << ')'  << "</h3>" << endl;
	exit(1);
}

void
UpdateFolderstat (const char * folder, const char * pUser,
	int nMail, int nRead, time_t now)
{
	ZString stgDB = DIR_LOCAL;
	stgDB += pUser;
	stgDB += "/mailstat";
	datum key, data;
	DBM * pDB = dbm_open(stgDB.chars(), O_RDWR|O_CREAT|O_EXLOCK, 0644);
	if (pDB)
	{
		key.dptr = (char *)folder;
		key.dsize = strlen(folder);

		data = dbm_fetch(pDB, key);
		const char * p = data.dptr;
		time_t d_time = 0;
		int n = 0;
		int r = 0;
		if (p)
		{
			d_time = atoi(p);
			p = strchr(p, '\t');
			if (p)
			{
				p++;
				n = atoi(p);
				p = strchr(p, '\t');
				if (p)
				{
					p++;
					r = atoi(p);
				}
			}
		}

		if (n != nMail || r != nRead)
		{
			char buf[PATH_MAX];
			sprintf(buf, "%ld\t%d\t%d", now, nMail, nRead);

			key.dptr = (char *)folder;
			key.dsize = strlen(folder);

			data.dptr = buf;
			data.dsize = strlen(buf);

			dbm_store(pDB, key, data, DBM_REPLACE);
		}
		dbm_close(pDB);
	}
}

int
main (int ac, char **av)
{
	//
	// Determine foldername
	//
	bool buildin = true;
	char * foldername = strrchr(av[0], '/');
	if (foldername == NULL)
		foldername = av[0];
	else
		++foldername;

	if (strcmp(foldername, "folder") == 0)
	{
		++av;
		--ac;
		foldername = av[0];
		buildin = false;
		if (*foldername == '/')
			++foldername;
	}

	// sanity check... only alphanumeric?
	for(char * p = foldername; *p; p++)
	{
		if (isupper(*p)) *p = tolower(*p);
		else if (!islower(*p) && !isdigit(*p) && *p != '/')
		{
			//goto badmbox;
			cerr << "Illegal Mail Box " << foldername << endl;
			exit (1);
		}
	}

	//
	// Determine User
	//
	char * pUser = getenv("REMOTE_USER");

	// Need before CmdOption setup
	// => -X must be the 1st option
	int outType = ((ac>1) && (strcmp(av[1], "-X") == 0)) ? 1 : 0;

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
		HTMLBeg(cout, outType) << DEF_BODY_TAG << endl
		     << "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
		     << " width=200 height=31>";
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

	//
	// Determine Option
	//
	string progname("/folder?");
	progname += foldername;
	MailOption options(path_info?0:ac,
				path_info?&path_info:av,
				progname.c_str(), pUser);

	MailBox * pbx = (strcmp(foldername, "in") == 0)
		? new InBox(stgUser)
		: new MailBox(stgUser, foldername);

	outType = options.IsXML() ? 1 : 0;

	MailCmd cmd;
	switch((cmd = options.GetCommand()))
	{
#ifdef USENET_APPROVE
	case CMD_APPROVE:
	  {
		HTMLBeg(cout, outType) << "<head>" << endl
			<< "<title>Approve News Article- Direct Read "
			<< "Email - WebMail Interface</title>" << endl
			<< "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
			<< " width=200 "
			<< "height=31><p>"
		;

		if(!pbx->GetCurMailInfo(options.GetMailIdx(),
			options.GetMailID()))
		{
			cout
				<< "Cannot locate the email message. It might "
				<< "be deleted or transferred to another "
				<< "mailbox." << endl
				<< "</body>" << endl << "</html>" << endl;
			exit(0);
		}

		FILE * tmpfp = NULL;
		char tmpf[] = "/tmp/apXXXXXX";
		if (mktemp(tmpf))
			tmpfp = fopen(tmpf, "w");

		if (tmpfp == NULL)
		{
			cout
				<< "Error in approving news article."
				<< " Please contact webmaster." << endl
				<< "</body>" << endl << "</html>" << endl;
			exit(0);
		}

		struct MsgInfo * pInfo = pbx->GetInfo();
		caddr_t p = pbx->GetMsg(pInfo->msgOffset);
		int nLen = pInfo->msgLength;
		MailSettings settings(stgUser);

		string approve = "/usr/sbin/sendmail -i -t -f ";
		approve += pUser;
		approve += "@";
		approve += 
		approve += " < ";
		approve += tmpf;

		const char * from = settings.GetAlias();
		fprintf(tmpfp, "To: forward-post@newspost.newsadm.com\n");
		fprintf(tmpfp, "From: %s\n", from);
		fprintf(tmpfp, "Subject: Approve News Article\n\n");
		
		
		bool nl = true;
		for (const char * q = p+nLen; p < q; p++)
		{
			if (nl) putc('>', tmpfp);
			putc(*p, tmpfp);
			nl = (*p == '\n');
		}
		fclose(tmpfp);

		system (approve.c_str());
#if 0
		{
			cout
				<< "Error in approving news article."
				<< " Please contact webmaster." << endl
				<< "</body>" << endl << "</html>" << endl;
			exit(0);
		}
#endif

		if (tmpf) unlink(tmpf);

		cout
			<< "News Article send to news server." << endl
			<< "</body>" << endl << "</html>" << endl;
		HTMLEnd(cout);
		break;
	  }
#endif

	case CMD_XFER:
	  {
		HTMLBeg(cout, outType);
		if(!pbx->GetCurMailInfo(options.GetMailIdx(),
			options.GetMailID()))
		{
			cout << DEF_BODY_TAG << endl
				<< "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
				<< "width=200 "
				<< "height=31><p>"
				<< "Cannot locate the email message. It might "
				<< "be deleted or transferred to another "
				<< "mailbox." << endl
				<< "</body>" << endl << "</html>" << endl;
			delete pbx;
			return(0);
		}

		struct MsgInfo * pInfo = pbx->GetInfo();
		caddr_t pMap = pbx->GetMsg(pInfo->msgOffset);
		int nLen = pInfo->msgLength;
		const char * xferbox = options.GetXferTo();

		if (strcmp(xferbox, foldername) == 0)
		{
			cout << DEF_BODY_TAG << endl
				<< "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
				<< "width=200 "
				<< "height=31><p>"
				<< "Source and destinated mailboxes are "
				<< "identical, cannot transfer" << endl
				<< "</body>" << endl << "</html>" << endl;
			delete pbx;
			return(0);
		}

		if (strcmp(xferbox, "in") == 0)
		{
			InBox xbox(stgUser);
			xbox.AppendMail(pMap, nLen);
		}
		else
		{
#if 0
			// sanity check for mailbox name
			cout << DEF_BODY_TAG << endl
				<< "<img border=0 src=" << DIR_IMG << "drmlogo.gif "
				<< "width=200 "
				<< "height=31><p>"
				<< "Illegal destinated mailbox. Cannot "
				<< "transfer mail" << endl
				<< "</body>" << endl << "</html>" << endl;
			delete pbx;
			return(0);
#endif

			MailBox xbox(stgUser, xferbox);
			xbox.AppendMail(pMap, nLen);
		}

		pbx->DeleteMail(pMap, nLen);
	  }
		goto DspList;

	case CMD_DSPLIST:
		HTMLBeg(cout, outType);

		const char * pPopName;
		int nPOP;
		for (nPOP = 1; nPOP <= MAX_POP; nPOP++)
		{
			pPopName = options.GetPOPName(nPOP);
			if (pPopName[0]
			    && (strcasecmp(pPopName, foldername) == 0))
				break;
		}

		if((nPOP <= MAX_POP) && options.IsPOP(nPOP))
		{
			const char * POPservers =
				options.GetPOPServers(nPOP);
			const char * POPauths =
				options.GetPOPAuths(nPOP);

			try
			{
				POPAccount server(POPservers, POPauths,
					outType?NULL:&cout);
				string folddir = DIR_LOCAL;
				folddir += pUser;
				folddir += "/mail/";
				folddir += foldername;
				debug = 1;
				server.Pull(pUser, folddir.c_str());
				debug = 0;
			}
			catch(const char * p)
			{
				ErrorExit(POPservers, p);
			}
			catch(int err)
			{
				ErrorExit(POPservers, strerror(errno));
			}
		}
		
		goto DspList;

	case CMD_DELETE:
	  {
		MailBox xbox(stgUser, "/trash");
		xbox.DeleteBox();
	  }

		HTMLBeg(cout, outType);
		// FALL-THROUGH to DspList

	DspList:
		if (outType == 0)
	        {
		    cout << "<head>" << endl
			<< "<title>" << toupper(foldername[0])
			<< &foldername[1]
			<< " Box - Direct Read "
			<< "Email - WebMail Interface</title>" << endl
			<< "</head>" << endl << DEF_BODY_TAG << endl
		    ;
		}

		bool inbox = (strcasecmp(foldername, "in") == 0);
		MailSettings *settings = NULL;
 		if (inbox)
		{
		    settings = new MailSettings(stgUser);
		    if (settings->GetCheckTime() > 0)
		    {
		        if (outType == 0)
			{
 			     cout << "<META HTTP-EQUIV=\"Refresh\" CONTENT=\""
 				<< settings->GetCheckTime() * 60
 				<< ";\">" << endl;
			}
			else
			{
			}
		    }
		}

		time_t now = time(NULL);
		MailList & list = *((outType == 0) ?
			new MailListHTML(foldername, &options, pbx, stgUser) :
			new MailListData(foldername, &options, pbx, stgUser));
			//new MailListXML(foldername, &options, pbx, stgUser));
		cout << list;

 		if(inbox && !list.IsError() && pbx->IsNewMail() &&
 			settings->GetSoundSrc())
		{
		    if (outType == 0)
		    {
 			cout << "<EMBED SRC=\""
 				<< HTMLText(settings->GetSoundSrc())
 				<< "\" HIDDEN=TRUE AUTOSTART=TRUE LOOP=FALSE>"
 				<< endl;
		    }
		    else
		    {
			cout << "<IGotMail>" << HTMLText(settings->GetSoundSrc())
				<< "</IGotMail>" << endl;
		    }
		}

		UpdateFolderstat(foldername, stgUser.chars(), list.GetMail(),
			list.GetRead(), now);
//cerr << "folder.cpp: update " << foldername << " size " << list.GetMail() << ", " << list.GetRead() << endl;

		HTMLEnd(cout);
		delete settings;
		break;
	}

	delete pbx;
	return(0);
}
