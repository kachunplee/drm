#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <iostream.h>
#include <string.h>
#include <string>

#include "dmsg.h"

#include "cinet.h"

#include "inetudp.h"
#include "clientudp.h"

class desc_server
{
public:
	string name;
	u_short port;
};

//
// retry
//	> 0 -> No of retransmit
//	< 0 -> No retry, i.e. 1 transmit and |retry| = no sec to timeout
//		Mainly for localhost (localnet?) UDP socket.
//
ClientUDP::ClientUDP (const char * servers, u_short port, int retry, int buflen, int recheck)
	: m_nBuf(buflen), m_iBuf(sizeof(m_seqnum)), m_maxretry(retry)
{
	const char * p;
	int l,n;
	m_udp = NULL;
	m_nServer = 1;
	m_cServer = 0;
	m_recheck = 60;	// recheck in 120 sec if more than one servers
	m_tConnect = 0;

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

		DMSG(1, "%s:%d",
			m_servers[n].name.c_str(), m_servers[n].port);

		if(*p == ';') ++p;
	}

	m_pBuf = new char [buflen+4];
	m_seqnum = (unsigned) getpid() << 16;
}

ClientUDP::~ClientUDP ()
{
	delete m_udp;
	delete [] m_servers;
}

bool
ClientUDP::connect ()
{
	time_t now = time(NULL);
	int sServer = m_nServer;
	InetUDP * udp_s = NULL;

	if (m_udp == NULL)
	{
		// Retry??
		if ((m_tConnect > 0) && /* not if tried to connect before */
			((m_recheck == 0) /* or not requested */
				|| ((now - m_tConnect) <= m_recheck)))
					/* or recheck not time yet */
			return false;
	}
	else
	{
		// Connect to the primary server
		if (m_cServer == 0)
			return true;

		if ((m_recheck == 0)
			|| ((now - m_tConnect) <= m_recheck))
			return true;

		DMSG(1, "Retry primary servers");
		sServer = m_cServer;
		udp_s = m_udp;
		m_udp = NULL;
		m_cServer = 0;
	}

	if (m_udp == NULL)
		m_udp = new InetUDP(0, INADDR_ANY, m_maxretry);

	if (m_udp == NULL)
		return false;

	u_long addr;
	for(; m_cServer < sServer; m_cServer++)
	{
		addr = AtoInetAddr(m_servers[m_cServer].name.c_str());
		if(addr == INADDR_NONE)
		{
			DMSG(0, "Can't resolve %s",
				m_servers[m_cServer].name.c_str());
			continue;
		}

		SockAddrIN saddr((char *)&addr, m_servers[m_cServer].port);
		if(m_udp->Connect(saddr) >= 0)
		{
			// got one
			if (udp_s)
			{
				DMSG(1, "Switch server back to %s:%d",
					m_servers[m_cServer].name.c_str(),
					m_servers[m_cServer].port);
				delete udp_s;
			}
			else
				DMSG(1, "Connected to %s:%d",
					m_servers[m_cServer].name.c_str(),
					m_servers[m_cServer].port);

			m_tConnect = time(NULL);
			return true;
		}

		DMSG(0, "Can't connect to %s:%d",
			m_servers[m_cServer].name.c_str(),
			m_servers[m_cServer].port);
	}

	m_tConnect = time(NULL);

	if (udp_s)
	{
		// No previous server is avaliable yet
		// Use the connected one
		m_cServer = sServer;
		m_udp = udp_s;
		return true;
	}

	// No servers is avaliable
	DMSG(1, "No server avaliable");
	delete m_udp;
	m_udp = NULL;
	m_cServer = 0;
	return false;
}

void
ClientUDP::BegCmd ()
{
	m_iBuf = sizeof(m_seqnum);
}

int
ClientUDP::SetCmd (char * fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	m_iBuf += vsnprintf (m_pBuf+m_iBuf, m_nBuf-m_iBuf, fmt, args);
	return m_nBuf-m_iBuf;
}

char *
ClientUDP::SendRecv (char * rbuf, int *pl)
{
	if(!connect())
	{
		BegCmd();
		return NULL;
	}

	int n;
	char *pBuf;
	int nBuf;
	for(pBuf = m_pBuf, nBuf = m_iBuf;;)
	{
		*(unsigned *)m_pBuf = m_seqnum;
 		n = m_udp->SendRecv(pBuf, nBuf, rbuf, *pl);
		if(n < 0)
		{
			m_seqnum++;

			// Try next server
			++m_cServer;
			DMSG(1, "Trying next server");
			delete m_udp;
			m_udp = NULL;
			m_tConnect = 0;
			if(connect())
			{
				// set buffer to send packet to new server
				pBuf = m_pBuf;
				nBuf = m_iBuf;
				continue;
			}

			BegCmd();
			*pl = -1;
			return NULL;
		}

		n -= sizeof(m_seqnum);
		if((n >= 0)  && (m_seqnum == * (unsigned *) rbuf))
		{
			m_seqnum++;
			BegCmd();
			*pl = n;
			return rbuf+sizeof(m_seqnum);
		}

		DMSG(0, "Wrong sequence number %u<>%u",
			m_seqnum, *(unsigned*)rbuf);

		// no need to resend - just try receive another package
		pBuf = NULL;
		nBuf = 0;
	}
	/* NO BREAK */
}
