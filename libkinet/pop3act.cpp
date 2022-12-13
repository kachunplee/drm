#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmsg.h"
#include "cpop3.h"
#include "pop3act.h"

extern const char * POP3servers;
extern const char * POP3auths;

POP3Active::POP3Active ()
{
	char * p;
	m_pPOP3 = new CPOP3(POP3servers, POP3auths);

	p = m_pPOP3->SendCmd("LIST active\r\n");
	DMSG(1, "%s", p);
	if((p == NULL) || (strncmp(p, "215 ", 4)) != 0) throw(p);
}

POP3Active::~POP3Active ()
{
	delete m_pPOP3;
}

char * POP3Active::GetLine ()
{
	if(m_pPOP3 == NULL)
		return NULL;

	return m_pPOP3->GetLine();
}
