#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream.h>

#include "status.h"
#include "popacc.h"

extern int debug;

const int MAXERRMSG = 2048;
static char ErrMsg [MAXERRMSG];

const int INITMSGALLOC = 2048;

POPAccount::POPAccount (const char * servers, const char * auths,
	ostream * pstm)
	: CPOP3(servers, auths), m_status(pstm)
{
	m_msg = NULL;
	m_msgAlloc = 0;
	m_msgSize = 0;
	m_status("Connected to POP server: %s", servers);
}

POPAccount::~POPAccount ()
{
	delete [] m_msg;
}

int
POPAccount::Check (const char * user)
{
	char * p;
	uid_t uid;
	gid_t gid = (gid_t) -1;

	struct passwd *pw = getpwnam(user);
	if(pw == NULL)
	{
		snprintf(ErrMsg, MAXERRMSG, "No such user: %s", user);
		throw(ErrMsg);
	}
	uid = pw->pw_uid;

	struct group *grp = getgrnam("mail");
	if(grp)
		gid = grp->gr_gid;

	p = SendCmd("stat\r\n");
	if(strncmp(p, "+OK ", 4) != 0)
	{
		snprintf(ErrMsg, MAXERRMSG, "POP STAT Error: %s", p);
		throw(ErrMsg);
	}

	int noMsg = atoi(p+4);
	if(noMsg == 0)
		return noMsg;

	m_status("Got %d mails", noMsg);

	string mailbox = "/var/mail/";
	mailbox += user;

	umask(0);
	int mailfd = open(mailbox.c_str(), O_RDWR|O_CREAT|O_EXLOCK, 0660);
	if(mailfd < 0)
	{
		if(errno == EWOULDBLOCK)
			snprintf(ErrMsg, MAXERRMSG,
				"Mailbox is locked... most likely is opened by someone ");
		else
			snprintf(ErrMsg, MAXERRMSG, "Cannot access mailbox: %s",
				strerror(errno));
		throw(ErrMsg);
	}

	// fix mailbox ownership
	if(fchown(mailfd, uid, gid) < 0)
	{
		snprintf(ErrMsg, MAXERRMSG, "Cannot chown mailbox: %s",
			strerror(errno));
		throw(ErrMsg);
	}

	FILE * mailfp = NULL;
	mailfp = fdopen(mailfd, "w+");
	if(mailfp == NULL)
	{
		close(mailfd);
		snprintf(ErrMsg, MAXERRMSG, "Cannot create mailbox: %s",
			strerror(errno));
		throw(ErrMsg);
	}
	
	off_t lastoff;
	for(int i = 1; i <= noMsg; i++)
	{
		m_status("Retrieving %d of %d mails", i, noMsg);
		SetCmd("retr %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK ", 4) != 0)
		{
			fclose(mailfp);
			snprintf(ErrMsg, MAXERRMSG, "POP RETR Error: %s", p);
			throw(ErrMsg);
		}

		// Ready to read mail
		lastoff = fseeko(mailfp, (off_t) 0, SEEK_END);

		p = GetLine();
		static char sz_return_path [] = "Return-Path:";
		time_t now = time(NULL);
		if(p && strncmp(p, sz_return_path,
			sizeof(sz_return_path)-1) == 0)
		{
			fprintf(mailfp, "From %s %s",
				p+sizeof(sz_return_path), ctime(&now));
		}
		else
		{
			fprintf(mailfp, "From Unknown %s", ctime(&now));
			fputs(p, mailfp);
			fputc('\n', mailfp);
		}

		int r;
		bool bBlank = false;
		while((p=GetLine()))
		{
			bBlank = (*p == 0);
			r = fputs(p, mailfp);
			if(r != EOF)
				r = fputc('\n', mailfp);
			if(r == EOF)
			{
				int putserr = errno;

				fpurge(mailfp);
				ftruncate(mailfd, lastoff);
				fclose(mailfp);
				snprintf(ErrMsg, MAXERRMSG, "Error writing mailbox: %s",
					strerror(putserr));
				throw(ErrMsg);
			}
		}

		// Last line must be blank to separate mail in mailbox
		if(!bBlank)
			fputc('\n', mailfp); // if not, add one

#if 0
		// Delete mail
		SetCmd("dele %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK ", 4) != 0)
		{
			fclose(mailfp);
			snprintf(ErrMsg, MAXERRMSG, "POP DELE Error: %s", p);
			throw(ErrMsg);
		}
#endif
	}

	fclose(mailfp);
	return noMsg;
}

int
POPAccount::AppendMsg (const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int max = m_msgAlloc - m_msgSize;
	if(max == 0)
		AllocMsg(m_msgAlloc ? 2*m_msgAlloc : INITMSGALLOC);

	int size;
	for(;;)
	{
		size = vsnprintf(m_msg+m_msgSize, max, fmt, args);
		if(size <= max)
			break;

		AllocMsg(2*m_msgSize);
	}
	return 0;
}

void
POPAccount::AllocMsg (int alloc)
{
	m_msgAlloc = alloc;
	char * buf = new char [m_msgAlloc];
	if(m_msgSize) memcpy(buf, m_msg, m_msgSize);
	delete m_msg;
	m_msg = buf;
}
