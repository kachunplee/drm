#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "linein.h"

const int BLOCK_SIZE = 8192;

LineIn::LineIn (int fd, int size)
{
	Init(fd, size);
}

LineIn::LineIn (const char * name, int size, int mode)
{
	Init(open(name, O_RDONLY|mode), size);
}

void
LineIn::Init (int fd, int size)
{
	m_fd = fd;
	m_nBlkSize = size ? size : BLOCK_SIZE;
	m_lnBuf = new char [m_nBlkSize*2];	// allocate 2 blocks

	m_eof = false;
	m_err = 0;
	m_off = 0;
	m_nLine = 0;
	m_lnBeg = m_lnEnd = m_lnBuf;

	m_endc = -1;
}

LineIn::~LineIn ()
{
	if(m_fd >= 0) close(m_fd);
	delete [] m_lnBuf;
}

bool
LineIn::Open (const char * name, int mode)
{
	if (m_fd >= 0)
		Close();
		
	m_fd = open(name, O_RDONLY|mode);
	return m_fd >= 0;
}

int
LineIn::Close ()
{
      int r = close(m_fd);
      m_fd = -1;
      return r;
}

int
LineIn::Rewind ()
{
    	if(lseek(m_fd, (off_t) 0, SEEK_SET) == -1)
		return errno; 

	m_eof = false;
	m_err = 0;
	m_err = 0;
	m_off = 0;
	m_nLine = 0;
	m_lnBeg = m_lnEnd = m_lnBuf;
	return 0;
}

//
// Note: '\n' may have been changed to '\0'
//
int
LineIn::Seek (off_t off)
{
	m_eof = false;
	if(off >= m_off && off < (m_off+(m_lnEnd-m_lnBuf)))
	{
		m_lnBeg = m_lnBuf + (off - m_off);
		return 0;
	}

	off_t b_off = off % m_nBlkSize;
    	if(lseek(m_fd, off - b_off, SEEK_SET) == -1)
		return (m_err = errno); 

	m_off = off - b_off;
	int i = read(m_fd, m_lnBuf, m_nBlkSize);
	if(i < 0)
		return (m_err = errno);

	m_lnEnd = m_lnBuf + i;
	m_lnBeg = m_lnBuf;
	if(i < b_off)
		return (m_err = ESPIPE);

	m_lnBeg += b_off;
	return 0;
}

char *
LineIn::GetLine (bool ret2long)
{
	char *p;
	do
	{
		int n = m_lnEnd - m_lnBeg;
		if(n > 0)
		{
			p = FindNL();
			if(p) return p;

			/* Cannot find nl, move buffer down */
			if(m_lnBeg > m_lnBuf)
				memmove(m_lnBuf, m_lnBeg, n);
			p = m_lnBuf + n;
		}
		else
			p = m_lnBuf;

		m_off += m_lnBeg - m_lnBuf;
		m_lnBeg = m_lnBuf;
		m_lnEnd = p;
		if(m_eof)
			return NULL;

		int i = read(m_fd, p, m_nBlkSize);
		if (i < 0)
		{
			m_err = errno;
			return NULL;
		}

		if (i == 0) {
			m_err = 0;
			m_eof = true;
			return NULL;
		}

		m_lnEnd += i;

		/* Try to find nl again */
		p = FindNL();
		if(p) return p;

		/* No NL -- discard that block */
		m_lnBeg = m_lnEnd;
	} while(ret2long);

	m_err = E2BIG;
	return NULL;
}

char *
LineIn::FindNL ()
{
	char *p, *q;

	for(p = m_lnBeg; p < m_lnEnd; p++)
	{
		m_endc = *p;
		if(m_endc == '\0' || m_endc == '\n')
		{
			*p = '\0';
			m_nLine = p - m_lnBeg;
			q = m_lnBeg;
			m_lnBeg = p + 1;
			m_err = 0;
    			return q;
		}
	}

	return NULL;
}
