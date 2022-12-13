#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>

#include "def.h"
#include "mailopt.h"
#include "mailto.h"
#include "maillist.h"
#include "mailmsg.h"
#include "mailsettings.h"
#include "inbox.h"

BOOL bLynx = FALSE;
uid_t UserID = 0; 
gid_t UserGID = 0; 
uid_t HttpID = 0; 
gid_t MailGID = 0; 

int
main (int ac, char **av)
{
	char * path_info = getenv("PATH_INFO");
	char * foldername = NULL;
	if (path_info)
	{
		if (*path_info == '/') ++path_info;
		foldername = path_info;
		char * pend = strchr(path_info, '/');
		if (pend)
		{
			*pend++ = 0;
			path_info = pend;
		}
	}
	else
	{
		if (ac > 1)
		{
			++av;
			--ac;
			foldername = av[0];
			if (*foldername == '/') ++foldername;
		}
	}

	if (foldername == NULL)
		foldername = "in";

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

	string progname("/message/");
	progname += foldername;

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

	MailOption options(path_info?0:ac,
				path_info?&path_info:av,
				progname.c_str(), pUser);

	MailBox * pbx = (strcmp(foldername, "in") == 0)
		? new InBox(stgUser)
		: new MailBox(stgUser, foldername);

	MailCmd cmd;
	switch((cmd = options.GetCommand()))
	{
#ifdef USENET_APPROVE
	case CMD_APPROVE:
	  {
		if(!pbx->GetCurMailInfo(options.GetMailIdx(),
			options.GetMailID()))
		{
			HTMLBeg(cout) << "<head>" << endl
				<< "<title>Approve News Article- "
				<< "Direct Read "
				<< "Email - WebMail Interface</title>" << endl
				<< "</head>" << endl
				<< "<body>" << endl << "<p>"
			;

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
			HTMLBeg(cout) << "<head>" << endl
				<< "<title>Approve News Article- "
				<< "Direct Read "
				<< "Email - WebMail Interface</title>" << endl
				<< "</head>" << endl
				<< "<body>" << endl << "<p>"
			;

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
		approve += MAILHOST;
		approve += " < ";
		approve += tmpf;

		const char * from = settings.GetAlias();
		fprintf(tmpfp, "To: forward-post\n");
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
			HTMLBeg(cout) << "<head>" << endl
				<< "<title>Approve News Article- "
				<< "Direct Read "
				<< "Email - WebMail Interface</title>" << endl
				<< "</head>" << endl
				<< "<body>" << endl << "<p>"
			;

			cout
				<< "Error in approving news article."
				<< " Please contact webmaster." << endl
				<< "</body>" << endl << "</html>" << endl;
			exit(0);
		}
#endif

		if (tmpf) unlink(tmpf);

		cout <<
			"Content-Type: text/html\n\n"
			"<html>\n"
			"<head>\n"
			"<script language=javascript>\n"
			"<!--\n"
			"parent.DRMMsgBody.document.open();\n"
			"parent.DRMMsgBody.document.close();\n"
			"top.window.close();\n"
			"//-->\n"
			"</script>\n"
			"</head>\n"
			"<body>\n"
			"<h2>Message is sent</h2>\n"
			"<a href=javascript:top.window.close()>Close</a>\n"
			"</body>\n"
			"</html>\n"
		;
		break;
	  }
#endif

	default:
		MailMessage message(foldername, options, pbx, stgUser,
			cmd == CMD_DSPHDR);
		cout << message;
		if(options.GetAttNum() == NULL)
		{
			struct MsgInfo * pInfo = pbx->GetInfo();
			if(pInfo && !message.IsStatus(options.GetUpdStatus()))
				pbx->UpdStatus(pInfo, options.GetUpdStatus(),
				options.IsPreview());
		}
	}

	delete pbx;
	return(0);
}
