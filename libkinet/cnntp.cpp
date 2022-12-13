#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <signal.h>

#include "dmsg.h"
#include "cnntp.h"


int CNNTP_GETLINE_TIMEOUT = 5*60;

CNNTP::CNNTP (const char * servers, const char * auths, int port)
	: ClientTCP(servers, port)
{
	m_bConnected = false;
	m_bLongLine = false;

	char * p = SendCmd(NULL, "200 ");
	DMSG(1, "CNNTP: %s", p);

	if (p == NULL)
		return;

	char * q = strchr(p+4, '(');
	if (q)
	{
		char * r = strchr(q, ')');
		if (r) *r = 0; 
		m_conninfo = q+1;
		if (r) *r = ')'; 
	}
	else
		m_conninfo = p+4;

	if ((strstr(p, "posting") == NULL)
			&& (strstr(p, "Tornado") == NULL))
	{
		char * p = SendCmd("mode reader\r\n", "200 ");
		DMSG(1, "CNNTP: %s", p);
	}

	int n = GetCServer();
	if(n >= 0 && auths)
	{
		string userid;
		string passwd;
		for(int i = 0; (i < n) && *auths; auths++)
		{
			if(*auths == ';') i++;
		}

		if(*auths)
		{
			const char * q = auths;
			while(*auths && *auths != '/' && *auths != ';') auths++;
			userid = string(q, auths-q);
			if(*auths == '/')
			{
				q = ++auths;
				while(*auths && *auths != ';') auths++;
				passwd = string(q, auths-q);
			}
		}

		if(userid.length())
		{
			SetCmd("authinfo user %s\r\n", userid.c_str());
			p = SendCmd(NULL, "381 ");
			if (p == NULL)
				return;

			SetCmd("authinfo pass %s\r\n", passwd.c_str());
			p = SendCmd();
		}
	}

	m_bConnected = true;
}

CNNTP::~CNNTP ()
{
	if (IsConnect())
	{
		char * p = SendCmd("QUIT\r\n");
		DMSG(1, "%s", p);
	}
}

char *
CNNTP::GetLine (int * pn)
{
	if (!m_bConnected)
		return NULL;

	char * p = NULL;

	//gotalarm = false;
	//sig_t oldsig = signal(SIGALRM, settimeout);
	//alarm(CNNTP_GETLINE_TIMEOUT);	// few minute to get a line

	p =  ClientTCP::GetLine(pn);
	
	//alarm(0);
	//signal(SIGALRM, oldsig);
	//if (gotalarm)
	//{
	//	DMSG(0, "CNNTP GetLineRaw timeout... possible error %s",
	//		strerror(errno));
	//	return NULL;
	//}

	return p;
}

char *
CNNTP::GetLineRaw ()
{
	int n;
	char * p;

	if (m_bLongLine)
	{
		// discard until EOL
		while((p = GetLine(&n)) != NULL)
			if (IsEOL()) break;

		m_bLongLine = false;

		if (p == NULL)
			return NULL;
	}

	p = GetLine(&n);

	if (p == NULL)
		return NULL;

	if (IsEOL())
	{
		if (p[n-1] == '\r') p[--n] = 0;
	}
	else
	{
		if (n > 1024)
			p[(n = 1024)] = 0;	// truncate
		m_bLongLine = true;		// and discard rest of line
	}
	return p;
}

char *
CNNTP::GetLine ()
{
	char * p = GetLineRaw();
	if (p == NULL)
		return NULL;

	if(p[0] == '.')
	{
		switch(p[1])
		{
		case '.':
			return p+1;

		case 0:
			return NULL;
		}
	}

	return p;
}

char *
CNNTP::SendCmd (char * buf, char * OKcode, int * pLen)
{
	char * p = NULL;
	p = ClientTCP::SendCmd(buf, OKcode, pLen);

	return p;
}
