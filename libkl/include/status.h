#ifndef __STATUS_H__
#define __STATUS_H__

#include <string>
#include <iostream.h>

class Status
{
public:
	Status (ostream * pstm) : m_pstm(pstm)
	{ m_bChg = false; m_pstmsave = NULL; }
	~Status();

	void operator () (const char * fmt, ...);

	void SetDefault();
	ostream *	GetSTM () const				{ return m_pstm; }
	void PushSTM (ostream * pstm = NULL)
		{ m_pstmsave = m_pstm; m_pstm = pstm;}

	void SetFunc(const char * f);

protected:
	ostream *	m_pstm;
	ostream *	m_pstmsave;
	bool		m_bChg;
	string		m_func;
};

#endif // __STATUS_H__
