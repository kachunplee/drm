#ifndef __MAILLIST_H__
#define __MAILLIST_H__

#include <string>
#include <time.h>

class MailBox;

class MailEntry
{
public:
	int		m_idx;
	int		m_status;
	int		m_size;

	string		m_msgid;
	string		m_date;
	string		m_who;
	string		m_subject;
	bool		m_hasATT;
};

class MailList
{
protected:
	const char *	m_name;
	string		m_boxName;
	MailOption *	m_pOpt;
	MailBox *	m_pMailbox;
	const char *	m_pUser;
	BOOL		m_bError;
	int		m_nOut;

	int		m_mail;
	int		m_read;

public:
	MailList(const char * name, MailOption * pOpt, MailBox* pBox, const char* pUser, int sortopt = 0);
	virtual ~MailList();

	friend ostream& operator <<(ostream&, MailList&);

	BOOL IsError ()				{ return m_bError; }
	int GetRead()				{ return m_read; }
	int GetMail()				{ return m_mail; }

protected:
	void PrintList(ostream&);

	virtual void NoMail(ostream&) = 0;
	virtual void MBoxBusy(ostream&) = 0;
	virtual void OutBegin(ostream&) = 0;
	virtual void OutEnd(ostream&) = 0;

	virtual void PrintLine(ostream&, MailEntry &ent) = 0;

	ostream & OutDate(ostream& stm, const char * p);
};

#endif //__MAILLIST_H__
