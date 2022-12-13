#include "display.h"

Display::Display ()
{
	m_out = &cout;
}

Display::~Display ()
{
	*m_out << "\n" "</body>" "</html>" "\n";
}
