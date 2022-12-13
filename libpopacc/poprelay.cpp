#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <string>

#include "dmsg.h"
#include "cpopacc.h"

string errbuf;

static const char *
ErrorExit (const char * p, const char * errmsg)
{
	errbuf = errmsg;
	DMSG (0, "PopRelay: fatal error: %s: %s\n", p, errmsg);
	return errbuf.c_str();
}

extern "C" const char *
PopRelay (const char * pUser,
	const char * POPservers, const char * POPauths)
{
	try
	{
		POPAccount pop(POPservers, POPauths);
		pop.Pull(pUser);
	}
	catch(const char * p)
	{
		return ErrorExit(POPservers, p);
	}
	catch(int err)
	{
		ErrorExit(POPservers, strerror(errno));
	}

	return NULL;
}
