#include <sys/types.h>

#include "uudecode.h"

#define	DEC(c)	(((c) - ' ') & 077)		/* single character decode */

bool
UUDecode::Decode (const char * p, int maxchar)
{
	int n;
	if ((n = DEC(*p)) <= 0)
		return false;

	if(maxchar && (maxchar < (n+2)))
		return false;

	int nLen = 0;
	for (++p; n > 0; p += 4, n -= 3)
	{
		if (n >= 3)
		{
			m_pBuffer[nLen++] = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
			m_pBuffer[nLen++] = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
			m_pBuffer[nLen++] = DEC(p[2]) << 6 | DEC(p[3]);
		}
		else
		{
			if (n >= 1)
				m_pBuffer[nLen++] = DEC(p[0]) << 2 | DEC(p[1]) >> 4;
			if (n >= 2)
				m_pBuffer[nLen++] = DEC(p[1]) << 4 | DEC(p[2]) >> 2;
			if (n >= 3)
				m_pBuffer[nLen++] = DEC(p[2]) << 6 | DEC(p[3]);
		}
	}

	m_pBuffer += nLen;
	return true;
}
