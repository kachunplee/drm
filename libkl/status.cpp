#include <stdarg.h>

#include "status.h"

const char begJavaScript [] = "<script language=javaScript>";
const char endScript [] = "</script>";

Status::~Status ()
{
	SetDefault();
}

void
Status::SetDefault ()
{
	if(!m_bChg)
		return;

	if(m_pstm == NULL)
		m_pstm = m_pstmsave;

	if(m_pstm == NULL)
		return;

	*m_pstm << begJavaScript;
	if(m_func.length() == 0)
	    *m_pstm << "status = defaultStatus; \n";
	else
	    *m_pstm << m_func << "('');\n";

	*m_pstm << endScript;
	m_bChg = false;
}

void
Status::SetFunc (const char * func)
{
	m_func = func;
}

void
Status::operator () (const char * fmt, ...)
{
	if(m_pstm == NULL)
		return;

	bool sendnl = false;
	va_list args;
	va_start(args, fmt);
	*m_pstm << begJavaScript;
	if(m_func.length() == 0)
		*m_pstm << "status = \"";
	else
	    	*m_pstm << m_func << "(\"";

	if (fmt[0] == '\n')
	{
		sendnl = true;
		++fmt;
	}

	m_pstm->vform(fmt, args);
	*m_pstm << "\"";
	if(m_func.length() != 0)
		*m_pstm << ")";

	*m_pstm << "; " << endl << endScript;
	if (sendnl || m_func.length()) *m_pstm << endl;
	m_bChg = true;
}
