#ifndef __MAILLHTML_H__
#define __MAILLHTML_H__

#include "maillist.h"

class MailListHTML : public MailList
{
public:
	MailListHTML (const char * name, MailOption * pOpt, MailBox* pBox, const char* pUser, int sortopt = 0)
	   : MailList(name, pOpt, pBox, pUser, sortopt)
	{
	}

protected:
	virtual void NoMail(ostream&);
	virtual void MBoxBusy(ostream&);
	virtual void OutBegin(ostream&);
	virtual void OutEnd(ostream&);

	virtual void PrintLine(ostream&, MailEntry &ent);
};

#endif __MAILLHTML_H__
