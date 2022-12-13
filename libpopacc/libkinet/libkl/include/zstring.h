#ifndef __ZSTRING_H__
#define __ZSTRING_H__

#include <fstream.h>
#include <string>

class zstring : public string
{
	friend istream & operator>> (istream &is, zstring &s);
};

typedef istream_iterator<string> string_input;

#endif // __ZSTRING_H__
