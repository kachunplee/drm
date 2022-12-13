#ifndef __MAILTO_H__
#define __MAILTO_H__

class ZMIME;
class MailTo
{
protected:
	MailOption *	m_pOpt;
	ZString		m_szUser;
	char *		m_pMail;
	char *		m_pDate;
	char *		m_pFrom;
	char *		m_pReturn;
	char *		m_pSubject;
	char *		m_pTo;
	char *		m_pStatus;
	char *          m_pFoldername;
	BOOL		m_bHeader;
	BOOL		m_bHTML;
	struct MsgInfo * m_pMsgInfo;

public:
	MailTo(MailOption *, const char *, 
		struct MsgInfo *, caddr_t, char *, bool);
	~MailTo();

	friend ostream& operator <<(ostream&, MailTo&);

	BOOL IsStatus(char);

protected:
	ostream& OutHeader(ostream&);
	ostream& OutBody(ostream&);
	void OutBegin(ostream&);
	void Parse();
	bool CheckIfHTML(ZMIME &);
	void PrintBody(ostream&, ZMIME&);
};

#endif //__MAILTO_H__
