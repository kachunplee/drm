#ifndef __CPOPACC_H__
#define __CPOPACC_H__

#include "status.h"
#include "cpop3.h"

class POPAccount : public CPOP3
{
public:
	POPAccount(const char * servers, const char * auths,
		ostream * pstm = NULL);
	~POPAccount();

	int Pull(const char * user, const char * pBoxName = NULL);

protected:

protected:
	Status	m_status;
};

#endif // __CPOPACC_H__
