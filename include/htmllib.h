#ifndef __HTMLLIB_H__
#define __HTMLLIB_H__

class HTMLText
{
protected:
	const char * m_stg;
	char * m_pText;
	char * m_alloc;
	bool    m_bJS;
	
public:
	HTMLText (const char * p, bool bJS = false)
	{ m_stg = p; m_pText = NULL; m_alloc = NULL; m_bJS = bJS; }

	HTMLText(const char * p, int n, bool bJS = false);

	~HTMLText ()			{ delete m_pText; delete [] m_alloc; }

	friend ostream& operator <<(ostream&, const HTMLText&);
	const char * GetText ();
};

extern ostream& HTMLBeg(ostream & stm, int type = 0);
extern ostream& HTMLEnd(ostream & stm);
extern ostream & OutHTMLStg(ostream &stm, const char * stg, bool bJS = false);
extern ostream & OutJSStg(ostream & stm, const char * p, bool bQuote = true);

#endif // __HTMLLIB_H__
