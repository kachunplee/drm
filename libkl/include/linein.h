#ifndef __LINEIN_H__
#define __LINEIN_H__

#include <sys/types.h>

class LineIn
{
public:
	LineIn(const char * name, int blksize = 0, int mode = 0);
	LineIn(int fd = 0, int blksize = 0);
	~LineIn();

	bool Open(const char * name, int mode = 0);
	bool IsOpen ()			{ return m_fd >= 0; }
	int Close();

	int Rewind();
	int Seek(off_t off);
	off_t Tell()			{ return m_off + (m_lnBeg-m_lnBuf); }
	char * GetLine(bool ret2long = false);
	int GetLineLen () const		{ return m_nLine; }

	int GetErrno () const		{ return m_err; }
	int GetFileno () const		{ return m_fd; }
	int GetLength () const		{ return m_nLine; }
	char * GetBegin () const	{ return m_lnBeg; }
	char GetEndc () const		{ return m_endc; }

protected:
	void Init(int fd, int blksize);
	char * FindNL();

protected:
	int		m_fd;
	off_t		m_off;
	int		m_err;
	bool		m_eof;

	int		m_nLine;
	char *		m_lnBuf;
	int		m_nBlkSize;
	char *		m_lnBeg;
	char *		m_lnEnd;

	char		m_endc;
};

#endif  // __LINEIN_H__
