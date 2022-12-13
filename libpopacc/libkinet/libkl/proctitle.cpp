#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "proctitle.h"


ProcessTitle::ProcessTitle ()
{
	TimeStamp();
	m_start = m_stamp;
}

ProcessTitle::ProcessTitle (const char * mid, const char * cid, const char *uid)
{
	TimeStamp();
	m_start = m_stamp;
	SetUser(mid, cid, uid);
}

void
ProcessTitle::SetUser (const char * mid, const char * cid, const char *uid)
{
	if (mid && *mid)
	{
		m_user = mid;
		m_user += ' ';
	}

	if (cid && *cid)
	{
		m_user += cid;
		m_user += ':';
	}

	if (uid && *uid)
	{
		m_user += uid;
	}

	OutStatus();
}

ProcessTitle::~ProcessTitle ()
{
	setproctitle(NULL);
}

void
ProcessTitle::TimeStamp ()
{
	time_t t = time(NULL);
	struct tm * now = localtime(&t);
	m_stamp.sec = now->tm_sec;
	m_stamp.min = now->tm_min;
	m_stamp.hour = now->tm_hour;
}

void
ProcessTitle::SetCmd (const char * cmd, const char * art)
{
	m_cmd = cmd;
	if (art)
	{
		m_cmd += ':';
		m_cmd += art;
	}

	OutStatus();
}

static const char sepstg [] = " ";
static const char nullstg [] = "";
void
ProcessTitle::OutStatus (const char * activity, const char * sub)
{
	const char * asep = sepstg;
	const char * ssep = sepstg;

	if (activity == NULL)
		activity = asep = nullstg;

	if (sub == NULL)
		sub = ssep = nullstg;
 
	setproctitle("-%02d:%02d:%02d %s %s%s%s%s%s",
		(int) m_stamp.hour, (int) m_stamp.min, (int) m_stamp.sec,
		m_user.c_str(), m_cmd.c_str(),
		asep, activity,
		ssep, sub);
}
