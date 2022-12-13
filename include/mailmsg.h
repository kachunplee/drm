#ifndef __MAILMSG_H__
#define __MAILMSG_H__

class MailBox;
class ZMIME;

class MailMessage
{
protected:
	const char *	m_foldername;
	MailOption&	m_opt;
	MailBox *	m_pMailbox;
	const char *	m_pUser;
	ZString		m_Status;
	BOOL		m_bDspHdrs;
	int 		m_DspImage;

	ZString		m_szWho;
	ZString		m_szDate;
	ZString		m_hdrWho;
	ZString		m_szReply;
	ZString		m_szCC;
	ZString		m_szReplyAll;
	ZString		m_szNewsgrp;
	BOOL		m_bModerate;

public:
	MailMessage (const char * fname, MailOption& opt, MailBox* pBox, const char* pUser,
		BOOL bDspHdrs)
		: m_foldername(fname), m_opt(opt), m_pMailbox(pBox),
			m_pUser(pUser), m_bDspHdrs(bDspHdrs)
	{
		m_DspImage = NTRUE;
		m_bModerate = false;
	}

	friend ostream& operator <<(ostream&, MailMessage&);
	ostream& operator ()(ostream&);

	BOOL IsStatus(char);
	BOOL IsDspImage();

protected:
	void OutBegin(ostream&, const char *, const char *, const char *, BOOL);
	void OutEnd(ostream&, BOOL);

	void PrintHdr(ostream&, struct MsgInfo*, caddr_t);
	void PrintMsg(ostream&, struct MsgInfo*, const char *);
	void PrintBody(ostream&, struct MsgInfo*, ZMIME &, const char * = "");
	void PrintAttLink(ostream&, const char *, ZMIME &, const char *);
	void PrintAttach(ostream&, MailMessage&);
	void PrintPlain(ostream&, const char * beg, const char * end);
	void PrintLine(ostream&, const char *, const char *);
	int PrintLink(ostream&, const char *, BOOL);
};

#endif //__MAILMSG_H__
