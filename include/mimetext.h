#ifndef __MIMETEXT_H__
#define __MIMETEXT_H__

class MimeText
{
public:
	MimeText::MimeText();
	MimeText::~MimeText();

	const char * GetText(const char * p);
	const char * GetCharset ()		{ return m_charset; }

	const char * DecodeText(const char * p, char * &o);

protected:
	char	*m_buf;
	int	m_szbuf;
	char	*m_charset;
	int	m_szcharset;
};

// Share this one... but beware of the return shared buffer
extern MimeText gMimeText;

#endif // __MIMETEXT_H__
