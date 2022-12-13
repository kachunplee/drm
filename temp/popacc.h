#ifndef __POPACC_H__
#define __POPACC_H__

#include "status.h"
#include "cpop3.h"

class POPAccount : public CPOP3
{
public:
	POPAccount(const char * servers, const char * auths, ostream * = NULL);
	~POPAccount();

	int Check(const char * user);
protected:
	int AppendMsg(const char * fmt, ...);
	void AllocMsg(int alloc);

protected:
	Status	m_status;

	char *	m_msg;
	int	m_msgAlloc;
	int	m_msgSize;
};

#endif // __POPACC_H__
