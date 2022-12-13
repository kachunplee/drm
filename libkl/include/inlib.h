#ifndef __INLIB_H__
#define __INLIB_H__

#include <time.h>

class INDateTime
{
protected:
	tm		m_tm;
	time_t		m_epoch;
	
public:
	INDateTime();
	INDateTime(const char * p);

	time_t GetTime ()	{ return m_epoch; }
	tm& time(int tz_sec = 0);
	void SetTZ(const char *);
	tm& localtime(int tz_sec = 0);
};

class INMailName
{
protected:
	const char * m_stg;
public:
	INMailName (const char * p)			{ m_stg = p; }

	string & EMailAddress(string &);
	string & RealName(string &);
};

#endif
