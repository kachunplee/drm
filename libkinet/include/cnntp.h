#ifndef __CNNTP_H__
#define __CNNTP_H__

#include <string>
#include "clienttcp.h"

class CNNTP :  public ClientTCP
{
public:
	CNNTP(const char * servers, const char * auths, int port=119);
	~CNNTP();

	bool IsConnected ()	{ return m_bConnected; }
	char * GetLine(int *);
	char * GetLine();
	char * GetLineRaw();
	char * SendCmd(char * = NULL, char * = NULL, int * = NULL);
	const char * GetConnInfo ()	{ return m_conninfo.c_str(); }

protected:
	bool m_bConnected;
	bool m_bLongLine;
	string	m_conninfo;
};

#endif // __CNNTP_H__
