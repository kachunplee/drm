#include "zio.h"

static char nullstg = 0;

char * Zifstream::GetLine (int * pLen, int term)
{
	get(m_buf, term);
	char * p = m_buf.str();
	m_buf.Terminate(pLen);
	return p?p:&nullstg;
}

bool Zifstream::NextLine ()
{
	if(good())
	{
		get();										// Throw away Terminator
		m_buf.ResetPut();
		return true;
	}

	return false;
}
