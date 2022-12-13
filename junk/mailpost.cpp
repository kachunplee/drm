#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>

#include "def.h"
#include "mailopt.h"
#include "inbox.h"
#include "cgi.h"

BOOL bLynx = FALSE;
uid_t UserID = 0; 
gid_t UserGID = 0; 
uid_t HttpID = 0; 
gid_t MailGID = 0; 

#define LINELEN 78

static char * WeekDays [] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

static char * Months [] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};


extern char * hextostr(char *, char *);

void OutErrorMsg (char * p)
{
	cout << "Content-Type: text/html" << endl << endl
		<< DEF_BODY_TAG << endl;
	cout << p << endl;
}

int main(int argc, char* argv[])
{
	if(argc < 5)
	{
		OutErrorMsg("<h2>Input format error...</h2>");
	}

	char * pUser = argv[1];
	char * pTmpfile = argv[2];
	char * pFolderName = argv[3];
	int mailIdx = atoi(argv[4]);
	char * pMailID = NULL;
	char * pMailStat = NULL;
	if(argc > 5)
	{
		pMailID = argv[5];
		pMailStat = argv[6];
	}

	char * pbuf = new char[strlen(pMailID)+1];
	if (pMailID && *pMailID && *pMailID != '<')
		pMailID = hextostr(pMailID, pbuf);

	struct passwd * pPwdEnt = getpwnam(pUser);
	if(pPwdEnt == NULL)
	{
		OutErrorMsg("<h2>The email account you use is invalid</h2>");
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

	Zifstream tmpFile(pTmpfile, ios::nocreate|ios::in);
	if(!tmpFile || !tmpFile.good())
	{
		OutErrorMsg("<h2>Internal error...</h2>\n<hr><h3>Please contact tech support</h3>");
		return(1);
	}

	string progname("/mailpost?");
	progname += pFolderName;
	MailOption options(1, NULL, progname.c_str(), pUser);

    {
	MailBox outBox(pUser, "out");
	outBox.WriteBegin();

	//
	//
	outBox.WriteData("From ", 5);
	outBox.WriteData(pUser, strlen(pUser));
	outBox.WriteData(" ", 1);

	INDateTime now;
	time_t t = now.GetTime();
	char * pTime = ctime(&t);
	outBox.WriteData(pTime, strlen(pTime));

	now.SetTZ(options.GetTZName());
	tm tnow = now.localtime();

	int tz_min = tnow.tm_gmtoff/60;
	char s[2] = {0, 0};
	if (tz_min < 0)
	{
		s[0] = '-';
		tz_min = -tz_min;
	}
	char dateBuf[256];
	sprintf(dateBuf, "%s, %2d %s %4d %02d:%02d:%02d %s%02d%02d (%s)\n",
		WeekDays[tnow.tm_wday], tnow.tm_mday,
		Months[tnow.tm_mon], tnow.tm_year+1900,
		tnow.tm_hour, tnow.tm_min, tnow.tm_sec,
		s, tz_min/60, tz_min%60, tnow.tm_zone); 

	outBox.WriteData("Date: ", 6);
	outBox.WriteData(dateBuf, strlen(dateBuf));

/*
	//
	time_t t = now.GetTime();
	char * pTime = ctime(&t);
	outBox.WriteData(pTime, strlen(pTime));

	outBox.WriteData("Date: ", 6);
	char * pDateBuf = new char[strlen(pTime)+1];
	strcpy(pDateBuf, pTime);
	char * pWeekDay = pDateBuf;
	char * pMon = strchr(pWeekDay, ' ');
	*pMon = 0;
	while(isspace(*++pMon));
	char * pMDay = strchr(pMon, ' ');
	*pMDay = 0;
	while(isspace(*++pMDay));
	pTime = strchr(pMDay, ' ');
	*pTime = 0;
	while(isspace(*++pTime));
	char * pYear = strchr(pTime, ' ');
	*pYear = 0;
	while(isspace(*++pYear));
	pYear[4] = 0;
	sprintf(dateBuf, "%s, %s %s %s %s -0700 (PDT)\n", pWeekDay, pMDay,
		pMon, pYear, pTime); 
	outBox.WriteData(dateBuf, strlen(dateBuf));
*/

	// save all the headers to the out box
	char * p;
	do
	{
		p = tmpFile.GetLine();
		if(*p == 0 || *p == '\r')
			break;
		outBox.WriteData(p, strlen(p));
		outBox.WriteData("\n", 1);
	} while(tmpFile.NextLine());
	outBox.WriteData("Status: OS \n\n", 13);

	// save the body to the out box
	while(tmpFile.NextLine())
	{
		if(tmpFile.peek() == EOF)
			break;
		p = tmpFile.GetLine();
		if(*p)
			outBox.WriteData(p, strlen(p));
		outBox.WriteData("\n", 1);
	}
	outBox.WriteEnd();
    }
	tmpFile.close();

	setuid(geteuid());
	ZString stgTmp = "/usr/bin/perl /var/local/bin/expandalias.pl ";
	stgTmp += pUser;
	stgTmp += " ";
	stgTmp += pTmpfile;
	stgTmp += " > /dev/null 2>&1";
	system(stgTmp);
//	fprintf(stderr, "mailpost(expandalias):  %d\n", system(stgTmp));
	stgTmp = pTmpfile;
	stgTmp += ".a";
	struct stat sb;
	char * p = NULL;
	if(stat(stgTmp, &sb) > -1)
	{
		int fd = open(stgTmp, O_RDONLY|O_EXLOCK);
		chown(stgTmp, UserID, UserGID);
		if(fd > -1)
		{
			p = new char[sb.st_size + 128];
			strcpy(p, "/usr/sbin/sendmail -n '");
			int nLen = strlen(p);
			read(fd, p+nLen, sb.st_size);
			nLen += sb.st_size;
			stgTmp = "' < ";
			stgTmp += pTmpfile;
			stgTmp += " > /dev/null 2>&1";
			strcpy(p+nLen, stgTmp);
		}
	}
	setuid(UserID);
	if(p)
	{
		system(p);
		delete [] p;
	}
	else
	{
		stgTmp = "/usr/sbin/sendmail -t < ";
		stgTmp += pTmpfile;
		stgTmp += " > /dev/null 2>&1";
		system(stgTmp);
	}

	stgTmp = pTmpfile;
	stgTmp += ".a";
	unlink(stgTmp);

	if(mailIdx > 0 && pMailID != NULL && strlen(pMailID) > 0 &&
		pMailStat != NULL)
	{
		options.SetMailIdx(mailIdx);
		options.SetMailID(pMailID);

		MailBox * pbx = (strcmp(pFolderName, "in") == 0)
			? new InBox(pUser)
			: new MailBox(pUser, pFolderName);

		struct MsgInfo * pInfo;
		if(pbx->GetCurMailInfo(mailIdx, pMailID))
		{
			if((pInfo = pbx->GetInfo()))
				pbx->UpdStatus(pInfo, *pMailStat,
				options.IsPreview());
		}
		delete pbx;
	}

	cout <<
		"Content-Type: text/html\n\n"
		"<html>\n"
		"<head>\n"
		"<script language=javascript>\n"
		"<!--\n"
		"window.close();\n"
		"//-->\n"
		"</script>\n"
		"</head>\n"
		"<body>\n"
		"<h2>Message is sent</h2>\n"
		"<a href=javascript:window.close()>Close</a>\n"
		"</body>\n"
		"</html>\n"
	;
	return(0);
}
