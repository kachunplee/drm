#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <iostream.h>

class Display
{
public:
	Display();
	~Display();

	Display& operator <<(const char * p)	{ *m_out << p; return *this; }
	Display& operator <<(int n)		{ *m_out << n; return *this; }

protected:
	ostream	* m_out;
};

#endif // __DISPLAY_H__
