#ifndef	__MAILOPT_H__
#define	__MAILOPT_H__

#define M_BODYFRM	'B'
#define M_HDRFRM	'H'
#define M_PRNMSG	'M'
#define M_XML		'X'
#define M_PREVIEW	'P'
#define M_APPROVE	'a'
#define M_DELETE	'd'
#define M_FORWARD	'f'
#define M_HEADER	'h'
#define M_MAIL 		'm'
#define M_POP 		'p'
#define M_REPLY		'r'
#define M_REPLYALL	'R'
#define M_XFER		'x'
#define M_SORT		's'

enum MailCmd
{
	CMD_DSPLIST = 0,
	CMD_DSPMSG,
	CMD_DSPHDR,
	CMD_SEND,
	CMD_XFER,
	CMD_DELETE,
	CMD_APPROVE,
};

enum MailSort
{
	SORT_DEF = 0,
	SORT_DATE_R,
	SORT_DATE_A,
	SORT_SUBJ_R,
	SORT_SUBJ_A,
	SORT_WHO_R,
	SORT_WHO_A,
};

#define MAXPOPS		3

struct st_POP
{
	ZString		popName;
	ZString		popServers;
	ZString		popAuths;
};

class MailOption
{
private:
	ZString		m_stgMailID;
	ZString		m_stgHexMailID;
	int		m_nMailIdx;
	ZString 	m_ProgName;
	ZString		m_ToAddr;
	int		m_nMailType;
	MailCmd		m_Command;
	char 		m_cStatus;
	ZString		m_XferTo;
	BOOL		m_bXML;
	BOOL		m_bPreview;
	BOOL		m_bPrnMsg;
	BOOL		m_bHeader;
	BOOL		m_bFolderFrame;
	BOOL		m_bPreviewFrame;
	ZString		m_ListFontSize;
	ZString		m_TZName;
	ZString		m_DateFormat;
	int		m_SortOpt;

	int		m_POPAccount;
	struct st_POP	m_POPs[MAXPOPS];

	char *		m_pAttNum;
	char *		m_pFileName;

public:
	MailOption(int, char **, const char *, const char *);

	void GetPOPConfig(const char *);
	BOOL IsPOP(int);

	const char * GetMailID ()		{ return m_stgMailID.chars(); }
	const char * GetHexMailID ()	{ return m_stgHexMailID.chars(); }
	void SetMailID (char * p)		{ m_stgMailID = p; }
	int GetMailIdx ()			{ return m_nMailIdx; }
	void SetMailIdx (int n)			{ m_nMailIdx = n; }
	const char * GetProgName ()		{ return m_ProgName.chars(); }
	const char * GetToAddr ()		{ return m_ToAddr.chars(); }
	int GetMailType ()			{ return m_nMailType; }
	MailCmd GetCommand ()			{ return m_Command; }
	char GetUpdStatus ()			{ return m_cStatus; }
	const char * GetXferTo ()		{ return m_XferTo.c_str(); }
	BOOL IsXML ()				{ return m_bXML; }
	BOOL IsPreview ()			{ return m_bPreview; }
	BOOL IsPrnMsg ()			{ return m_bPrnMsg; }
	BOOL IsHeader ()			{ return m_bHeader; }
	BOOL IsFolderFrame ()			{ return m_bFolderFrame; }
	BOOL IsPreviewFrame ()			{ return m_bPreviewFrame; }
	const char * GetListFontSize ()	{ return m_ListFontSize.chars(); }
	const char * GetTZName ()	{ return m_TZName.chars(); }
	const char * GetDateFormat ()	{ return m_DateFormat.chars(); }
	int GetPOPAccount ()			{ return m_POPAccount; }
	void SetPOPAccount (int n)		{ m_POPAccount = n; }
	const char * GetPOPName (int i)		{ return m_POPs[i-1].popName; }
	const char * GetPOPServers (int i)	{ return m_POPs[i-1].popServers; }
	const char * GetPOPAuths (int i)	{ return m_POPs[i-1].popAuths; }
	const char * GetAttNum ()		{ return m_pAttNum; }
	int GetSortOpt ()			{ return m_SortOpt; }

protected:
	void Init(const char *, const char *);
	void PrxPathInfo(char *);
	void SetCmd(char *);
	void SetOpt(int, char **);
};

#endif	//__MAILOPT_H__
