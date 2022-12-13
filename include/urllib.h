#ifndef __URLLIB_H__
#define __URLLIB_H__

class URLText
{
protected:
	const char * m_stg;
	char * m_pText;
	char * m_alloc;
	bool   m_bScript;
	
public:
	URLText (const char * p, bool bScript = false)
	{ m_stg = p; m_pText = NULL; m_alloc = NULL; m_bScript = bScript; }

	URLText(const char * p, int n, bool bScript = false);

	~URLText ()		{ delete m_pText; delete [] m_alloc; }

	friend ostream& operator <<(ostream&, const URLText&);
	const char * GetText();
};

class URLPostText
{
protected:
	const char *	m_stg;
	
public:
	URLPostText (const char * p)		{ m_stg = p; }

	friend ostream& operator <<(ostream&, const URLPostText&);
};

#endif // __URLLIB_H__
