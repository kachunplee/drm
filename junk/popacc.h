#ifndef __POPACC_H__
#define __POPACC_H__

#include "status.h"
#include "cpop3.h"

#ifndef MAX_POP
#define MAX_POP 3
#endif

class POPAccount : public CPOP3
{
public:
	POPAccount(const char * servers, const char * auths, ostream * = NULL);
	~POPAccount();

	int Check(const char * user, const char * pBoxName);
protected:
	int AppendMsg(const char * fmt, ...);
	int AllocMsg(int alloc);

protected:
	Status	m_status;

	char *	m_msg;
	int	m_msgAlloc;
	int	m_msgSize;
};

#endif // __POPACC_H__
