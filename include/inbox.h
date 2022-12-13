#ifndef __INBOX_H__
#define __INBOX_H__

#include "mailbox.h"

class InBox : public MailBox
{
protected:
	int m_fdPOP;

public:
	InBox(const char *);
	virtual ~InBox();

	virtual void DeleteBox();
	virtual void UpdStatus(struct MsgInfo*, char, BOOL);

protected:
	virtual BOOL LockMailFile(int&);
	virtual void UnlockFile(int&);
};

#endif //__INBOX_H__
