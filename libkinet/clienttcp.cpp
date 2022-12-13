#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include <iostream.h>
#include <string.h>

#include "dmsg.h"

#include "cinet.h"

#include "inettcp.h"
#include "clienttcp.h"

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff      /* -1 return */ 
#endif

class desc_server
{
public:
	string name;
	u_short port;
};

ClientTCP::ClientTCP (const char * servers, u_short port, int buflen)
	: m_nBuf(buflen), m_iBuf(0),
		m_nOut(0), m_iOut(0),
		 m_tcp(NULL), m_cServer(0)
{
	const char * p;
	int l,n;
	m_nServer = 1;
	for(p = servers; *p; p++)
		if(*p == ';') ++m_nServer;

	m_servers = new desc_server [m_nServer];

	for(n = 0, p = servers; *p; ++n)
	{
		l = strcspn(p, ":;");
		m_servers[n].name = string(p,l);
		m_servers[n].port = port;
		p += l;
		l = 0;
		if(*p == ':') {
			m_servers[n].port = atoi(++p);
			p += strcspn(p, ";");
		}

		DMSG(1, "ClientTCP: %s:%d",
			m_servers[n].name.c_str(), m_servers[n].port);

		if(*p == ';') ++p;
	}

	m_pBuf = new char [m_nBuf+1];
}

ClientTCP::~ClientTCP ()
{
	delete m_tcp;
	delete [] m_servers;
}

bool
ClientTCP::connect ()
{
	if(m_tcp)
		return true;

	m_tcp = new InetTCP(0, INADDR_ANY);

	u_long addr;
	for(; m_cServer < m_nServer; m_cServer++)
	{
		addr = AtoInetAddr(m_servers[m_cServer].name.c_str());
		if(addr == INADDR_NONE)
		{
			DMSG(0, "ClientTCP: Can't resolve %s",
				m_servers[m_cServer].name.c_str());
			continue;
		}

		SockAddrIN saddr((char *)&addr, m_servers[m_cServer].port);
		time_t begConnect = time(NULL);
		if(m_tcp->Connect(saddr) >= 0)
		{
			// got one
			m_tConnect = time(NULL);

			if (debug > 0)
			{
			    addr = ntohl(addr);
			    DMSG(1, "ClientTCP: connected to %s %u.%u.%u.%u:%d ",
				m_servers[m_cServer].name.c_str(),
				(unsigned) (addr>>24)&0xff, (unsigned) (addr>>16)&0xff,
				(unsigned) (addr>>8)&0xff, (unsigned) addr&0xff,
				m_servers[m_cServer].port);
			}
			return true;
		}

		addr = ntohl(addr);
		DMSG(0, "ClientTCP: Can't connect to %s %u.%u.%u.%u:%d timewait=%ld",
			m_servers[m_cServer].name.c_str(),
			(unsigned) (addr>>24)&0xff, (unsigned) (addr>>16)&0xff,
			(unsigned) (addr>>8)&0xff, (unsigned) addr&0xff,
			m_servers[m_cServer].port,
			(long) (time(NULL)-begConnect));
	}

	// No servers is avaliable
	DMSG(0, "ClientTCP: No server avaliable");
	delete m_tcp;
	m_tcp = NULL;
	return false;
}

int
ClientTCP::SetCmd (char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	m_iBuf += vsnprintf (m_pBuf+m_iBuf, m_nBuf-m_iBuf, fmt, args);
	return m_nBuf-m_iBuf;
}

char *
ClientTCP::SendCmd (char * buf, char * OKcode, int * pLen)
{
	if(!connect())
	{
		DMSG(0, "ClientTCP: Cannot connect");
		return NULL;
	}

	if(buf && buf[0])
		m_iBuf += snprintf (m_pBuf+m_iBuf, m_nBuf-m_iBuf, "%s", buf);

	if (m_iBuf > 0)
		DMSG(1, "ClientTCP::Send: %s", m_pBuf);
	m_nOut = m_tcp->SendRecv(m_pBuf, m_iBuf, m_pBuf, m_nBuf);
	m_iBuf = 0;
	if((m_nOut < 0) || (m_nBuf <= 0))
	{
		DMSG(0, "ClientTCP SendRecv Error: %s", strerror(errno));
		return NULL;
	}

	m_iOut = 0;
	char * p = GetLine(pLen);
	if(OKcode)
	{
		if((p == NULL) || (strncmp(p, OKcode, strlen(OKcode)) != 0))
		{
			DMSG(0, "ClientTCP: connect: bad code %s", (p?p:"(null)"));
			return NULL;
		}
	}

	return p;
}

char *
ClientTCP::GetLine (int * pLen)
{
	for(;;)
	{
		int nLen = m_nOut - m_iOut;
		if(nLen == 0)
			m_nOut = m_iOut = 0;
		else
		{
			char * p = NULL;
			m_isEOL = false;
			for(p = m_pBuf+m_iOut; p < &m_pBuf[m_nOut]; p++)
				if(*p == '\n')
				{
					m_isEOL = true;
					break;
				}

			// Find line or full buffer
			if(m_isEOL || (m_iOut == 0 && m_nOut == m_nBuf))
			{
				*p++ = 0;	// terminate string
				char * q = m_pBuf+m_iOut; // begin string
				if (m_isEOL)
					m_iOut = p-m_pBuf;
				else
					m_iOut = m_nOut;
				if (pLen)
					*pLen = p-q-1;
				DMSG(3, "ClientTCP::GetLine %s", q);
				return q;
			}

			DMSG(2, "ClientTCP: m_iOut=%d m_nOut=%d", 
					m_iOut, m_nOut);

			if(m_iOut > 0)
			{
				//
				// Move to front
				//
				if (m_nOut <= m_iOut)
				{
					if (m_nOut < m_iOut)
						DMSG(0, "ClientTCP: m_nOut (%d) < m_iOut (%d)", m_nOut, m_iOut);
					m_nOut = 0;
				}
				else
				{
					m_nOut -= m_iOut;
					DMSG(2, "ClientTCP: move buffer %d byte", m_nOut);
					memmove(m_pBuf, m_pBuf+m_iOut, m_nOut);
				}
				m_iOut = 0;
			}

		}

		if(!connect())
			return NULL;

		if ((m_nBuf-m_nOut) <= 0)
		{
			DMSG(0, "ClientTCP: Buffer full?! nOut=%d, iOut=%d... reset nOut=iOut=0",
				m_nOut, m_iOut);
			m_nOut = m_iOut = 0;
		}

		int n = m_tcp->Recv(m_pBuf+m_nOut, m_nBuf-m_nOut);
		if(n < 0)
		{
			DMSG(0, "ClientTCP: Recv error: %s", strerror(errno));
			return NULL;
		}

		if(n == 0)
		{
		    if (nLen == 0)
			return NULL;

		    // Last partial line
		    char * q = m_pBuf+m_iOut;
		    m_pBuf[m_nOut] = 0;
		    if (pLen)
			*pLen = m_pBuf+m_nOut-q;
		    DMSG(3, "ClientTCP::GetLine %s", q);
		    m_iOut = m_nOut;
		    return q;
	        }

		m_nOut += n;
		DMSG(2, "ClientTCP: Recv: m_iOut=%d m_nOut=%d", 
					m_iOut, m_nOut);
	}
}

int
ClientTCP::Send (const char * buf, int len = 0)
{ 
	if(!connect())
		return -1;
	return m_tcp->Send(buf, len);
}
