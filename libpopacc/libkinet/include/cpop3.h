#ifndef __CPOP3_H__
#define __CPOP3_H__

#include <string>
#include "clienttcp.h"

class CPOP3 :  public ClientTCP
{
public:
	CPOP3(const char * servers, const char * auths);
	~CPOP3();

	char * GetLine(int * p)		{ return ClientTCP::GetLine(p); }
	char * GetLine();
};

#endif // __CPOP3_H__
