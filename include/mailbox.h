#ifndef __MAILBOX_H__
#define __MAILBOX_H__

enum MSTATUS
{
	MS_NOMAIL = 0,
	MS_BUSY,		// lock file is busy
	MS_BUSY1,		// data file is busy
	MS_MAIL,
	MS_MAPERR,
};

//
// mail status
//
#define MSG_NEW		0x01
#define MSG_READ	0x02
#define MSG_REPLY	0x04
#define MSG_FORWARD	0x08
#define MSG_SEND	0x10

class ZMIME;
struct MsgInfo
{
	int		msgStatus;
	int		msgLength;
	int		msgHeaderLength;
	off_t		msgOffset;
	off_t		msgStatOffset;		// offset of status header
	class ZMIME *	msgMsg;
};

struct headline {
	char	*l_from;	/* The name of the sender */
	char	*l_tty;		/* His tty string (if any) */
	char	*l_date;	/* The entire date string */
};

typedef map<caddr_t, int> Mails;

class MBoxLock;
class MailBox
{
protected:
	const char *	m_pUser;
	ZString		m_BoxFile;
	MSTATUS		m_MailStatus;

	int		m_fdMBox;
	caddr_t		m_pMMap;
	size_t		m_nMMap;

	struct MsgInfo	m_Info;
	BOOL		m_bNewMail;

	caddr_t		m_pCurMMap;
	int		m_nCurMail;

	MBoxLock	*m_lockMBox;

public:
	MailBox(const char * u, const char * mbx);
	virtual ~MailBox();

	virtual void DeleteBox();
	virtual void UpdStatus(struct MsgInfo*, char, BOOL);

	void SetFirstMail ()		{ SetCurMMap(0); m_nCurMail = 0; }
	BOOL GetNextMail(struct MsgInfo&, char *);

	BOOL GetMailInfo(struct MsgInfo&, int, const char *);
	BOOL GetCurMailInfo(int, const char *);
	caddr_t GetMsg (off_t o)	{ return GetMMap() + o; }
	bool AppendMail(const char *, int);
	void DeleteMail(caddr_t, int);
	void DeleteMail(Mails&);
	void RenameBox(const char *);
	void WriteBegin();
	void WriteData(const char *, int);
	void WriteEnd();

	MSTATUS GetMailStatus ()		{ MMap(); return m_MailStatus; }

	struct MsgInfo * GetInfo () 		{ return &m_Info; }
	BOOL IsNewMail () 			{ return m_bNewMail; }

protected:
	virtual BOOL LockMailFile(int&);
	virtual void UnlockFile(int&);

	bool MMap();
	void UnMMap();
	BOOL IsMMap ()				{ return m_pMMap!=NULL; }
	caddr_t GetMMap ()			{ MMap(); return m_pMMap; }
	void SetCurMMap (off_t n) 		{ m_pCurMMap = GetMMap() + n; }

	BOOL MapMailbox(int);
	BOOL IsHead(char *);
	void Parse(char *, struct headline *, char *);
	char * NextWord(char *, char *);
	char * CopyIn(char *, char **);
	int IsDate(char *);
	int Cmatch(char *, char *);
	int Frequent(char *, char);

	struct mime_message * MimeReadMessage(char *, int, const char *);
	struct mime_message ** MimeParseMultiPart(char *, int, const char *, int *, char **, const char *);
};

#endif //__MAILBOX_H__
