#ifndef __CLIENTUDP_H__
#define __CLIENTUDP_H__

#include <time.h>

#include "inetudp.h"

class desc_server;
class ClientUDP 
{
public:
	ClientUDP(const char *, u_short port = 0,
		int retry = 3, int buflen = 2048, int recheck = 60);
	~ClientUDP();
	int Getfd () const	{ return m_udp ? m_udp->Getfd() : -1; }

	void SetRecheck (int n) { m_recheck = n; }

	void BegCmd();
	int SetCmd(char * fmt, ...);
	char * SendRecv (char *, int *);

protected:
	char *		m_pBuf;
	int		m_nBuf;
	int		m_iBuf;
	int		m_maxretry;

	InetUDP *	m_udp;
	desc_server *	m_servers;
	int		m_nServer;
	int		m_cServer;
	unsigned	m_seqnum;
	time_t		m_tConnect;

	int		m_recheck;	/* recheck in sec
						when not connect to 
						the first server */

protected:
	bool connect();
};

#endif // __CLIENTUDP_H__
