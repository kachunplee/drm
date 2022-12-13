#ifndef __PROCTITLE_H__
#define __PROCTITLE_H__

#include <string>

struct tclk
{
	short	sec;
	short	min;
	short	hour;
};

class ProcessTitle
{
public:
	ProcessTitle();
	ProcessTitle(const char * mid, const char * cid, const char *uid);
	~ProcessTitle();
	void SetUser(const char * mid, const char * cid, const char *uid);
	void SetCmd(const char * cmd, const char * arg = NULL);
	void OutStatus(const char * activity = NULL, const char * sub = NULL);

protected:
	string		m_user;
	string		m_cmd;
	tclk		m_start;
	tclk		m_stamp;

protected:
	void TimeStamp();
};

#endif // __PROCTITLE_H__
