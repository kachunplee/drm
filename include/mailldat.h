#ifndef __MAILLDAT_H__
#define __MAILLDAT_H__

#include "maillist.h"

class MailListData : public MailList
{
public:
	MailListData (const char * name, MailOption * pOpt, MailBox* pBox, const char* pUser, int sortopt = 0)
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

#endif __MAILLDAT_H__
