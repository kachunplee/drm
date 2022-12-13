#ifndef  __ZMIME_H__
#define  __ZMIME_H__

#include <string>
#include <vector>
#include <map>

class ZMIME
{
public:
	ZMIME(const char *, int);

	~ZMIME();
	const char * GetHeader(const char * h);
	const char * GetHeaderC(const char * h);
	const char * GetContentType();

	int GetBodyLen() 	{ return m_bodylen; }
	const char * GetBody() 	{ return m_body; }

	int GetPrologLen()	{ return m_prologlen; }
	const char * GetProlog()	{ return m_prolog; }

	int GetEpilogLen()	{ return m_epiloglen; }
	const char * GetEpilog()	{ return m_epilog; }

	void DecodeBody();

	int GetNumAtt() 	{ return m_attachments.size(); }

	ZMIME & GetAtt(int i);
	ZMIME * GetAtt(const char *);

	bool IsCID();
	bool IsPlain();
	bool IsHTML();
	bool IsUUEncoded();
	bool IsPlainAndHTML();

	bool HasAttachment();

	const char * GetAttFileName();
	const char * GetUUFileName();

	void SetCID();
	const char * MapContentID(const char *);
	void AddContentID(map<string,string> &, const char * = "");

protected:
	void GetMessage(char *msg, int len, const char *crlfpair);
	void ParseMultipart(char *msg, int len, const char *bd,
		char **endpos, const char *crlfpair);
	int EstimateRawSize();
	int GetRawBody(char *);

protected:
	vector<ZMIME *> m_attachments;
	int	 m_nattachments;

	struct mime_header *mm_headers;

	char *	m_body;
	int	m_bodylen;

	char *	m_prolog;
	int	m_prologlen;
	char *	m_epilog;
	int	m_epiloglen;

	bool	m_bDecoded;
	bool	m_cid;

	string	m_UUFile;
	string	m_AttFile;

	const char * m_contentType;

	map<string, string> * m_contentIDs;
};

#endif // __ZMIME_H__
