#ifndef __UUDECODE_H__
#define __UUDECODE_H__

class UUDecode
{
protected:
	char *			m_pBuffer;

public:
	UUDecode (char * buf)	{ m_pBuffer = buf; }
	bool Decode(const char *, int = 0);

	char * GetBuf ()	{ return m_pBuffer; }
};

#endif //__UUDECODE_H__
