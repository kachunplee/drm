#ifndef __CLIENTNNTP_H__
#define __CLIENTNNTP_H__

#include <string>
#include <vector>

typedef struct grp_info
{
	int numart;
	unsigned min;
	unsigned max;
} grpinfo;

class CNNTP;
class ClientNNTP
{
public:
	ClientNNTP(const char * servers, const char * auths);
	~ClientNNTP();

	char * command(const char * cmd, const char * av = NULL);
	grpinfo group(const char * group);
	char * xover(unsigned beg, unsigned end = 0, bool in = false);
	char * xhdr(const char * phdr, const char * prange, bool in = false);
	char * xpat(const char * phdr, const char * prange,
		const char * pattern, bool in = false);
	char * listgroup();
	char * article(const char * name, bool hdronly = false);
	char * article(const char * name, int t /* 0=all, 1=head, 2=body*/);

	bool IsConnect();
	void CloseArt();
	bool ArtRewind();
	const char * GetArtName();
	char * GetArtLine();
	int GetLnLen ()			{ return m_nLnLen; }
	int GetArtSize();
	char * GetArtMsgID();
	char * GetArtNewsgrps();

	const char * GetCurrentGroup () 	{ return m_cGroup.c_str(); }
	unsigned GetCurrentartnum () 		{ return m_cArtnum; }
	void SetCurrentArtnum (unsigned artnum) { m_cArtnum = artnum; }

	char * GetDataLine();
	bool IsInData ()			{ return m_dataState != IN_CLOSE; }
	char * GetLine(int * pn);
	char * GetLine();
	bool IsEOL();

	bool Close();

protected:
	char * GetHeader();
	void FlushData();

protected:
	CNNTP	*m_pNNTP;
	string	m_servers;
	string	m_auths;

	string  m_name;
	int	m_artcmd;

	string	m_cGroup;
	unsigned m_cArtnum;

	unsigned *m_headers;
	unsigned m_mHdr;
	unsigned m_nHdr;
	unsigned m_iHdr;

	int	m_oMsgID;
	int	m_oNewsgrps;
	int	m_oXref;

	int	m_nArtSize;
	int	m_nLines;

	int	m_nLnLen;
	char	*m_hdrBuf;
	unsigned m_offBuf;
	unsigned m_hbSize;

	enum { IN_CLOSE, IN_HEADER, IN_DATA } m_dataState;
};

#endif // __CLIENTNNTP_H__
