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

#include "conf.h"
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
POPAccount::Check (const char * user, const char * pBoxName)
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
	if(strncmp(p, "+OK", 3) != 0)
	{
		snprintf(ErrMsg, MAXERRMSG, "POP STAT Error: %s", p);
		throw(ErrMsg);
	}

	int noMsg = atoi(p+4);
	if(noMsg == 0)
		return noMsg;

	m_status("Got %d mails", noMsg);

	string mailbox = DIR_LOCAL;
	mailbox += user;
	mailbox += "/mail/";
	mailbox += pBoxName;

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

	off_t lastoff;
	for(int i = 1; i <= noMsg; i++)
	{
		m_status("Retrieving %d of %d mails", i, noMsg);
		SetCmd("retr %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK", 3) != 0)
		{
			close(mailfd);
			snprintf(ErrMsg, MAXERRMSG, "POP RETR Error: %s", p);
			throw(ErrMsg);
		}


		// Truncate message size
		m_msgSize = 0;

		// 1st line in the mailbox must be in format
		// 	From ...
		static char sz_return_path [] = "Return-Path:";
		time_t now = time(NULL);
		p = GetLine();
		if(p && strncmp(p, sz_return_path,
			sizeof(sz_return_path)-1) == 0)
		{
			AppendMsg("From %s %s",
				p+sizeof(sz_return_path), ctime(&now));
		}
		else
		{
			AppendMsg("From Unknown %s%s\n", ctime(&now), p);
		}

		// Get rest of message
		bool bBlank = false;
		while((p=GetLine()))
		{
			bBlank = (*p == 0);
			AppendMsg("%s\n", p);
		}

		// Last line must be blank to separate mail in mailbox
		if(!bBlank)
			AppendMsg("\n");

		// Ready to write mail
		lastoff = lseek(mailfd, (off_t) 0, SEEK_END);
		int n = write(mailfd, m_msg, m_msgSize);
		if(n != m_msgSize)
		{
			int putserr = errno;

			// Any error... throw away read in message
			ftruncate(mailfd, lastoff);
			close(mailfd);
			snprintf(ErrMsg, MAXERRMSG, "Error writing mailbox: %s",
				strerror(putserr));
			throw(ErrMsg);
		}

		// Delete mail
		SetCmd("dele %d\r\n", i);
		p = SendCmd();
		if(strncmp(p, "+OK", 3) != 0)
		{
			ftruncate(mailfd, lastoff);
			close(mailfd);
			snprintf(ErrMsg, MAXERRMSG, "POP DELE Error: %s", p);
			throw(ErrMsg);
		}
	}

	close(mailfd);
	return noMsg;
}

int
POPAccount::AppendMsg (const char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int max = m_msgAlloc - m_msgSize;
	if(max == 0)
		max = AllocMsg(m_msgAlloc ? 3*m_msgAlloc/2 : INITMSGALLOC);

	int size;
	for(;;)
	{
		size = vsnprintf(m_msg+m_msgSize, max, fmt, args);
		if(size < max)
		{
			m_msgSize += size;
			return size;
		}

		max = AllocMsg(3*m_msgAlloc/2);
	}
	/* NO BREAK */
}

int
POPAccount::AllocMsg (int alloc)
{
	m_msgAlloc = alloc;
	char * buf = new char [m_msgAlloc];
	if(m_msgSize) memcpy(buf, m_msg, m_msgSize);
	delete m_msg;
	m_msg = buf;
	return m_msgAlloc - m_msgSize;
}
