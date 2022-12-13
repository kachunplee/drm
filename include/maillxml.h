#ifndef __MAILLXML_H__
#define __MAILLXML_H__

#include "maillist.h"

class MailListXML : public MailList
{
public:
	MailListXML (const char * name, MailOption * pOpt, MailBox* pBox, const char* pUser, int sortopt = 0)
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

#endif __MAILLXML_H__
