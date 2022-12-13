#include "zstglib.h"

int
stg_index (string & s, const Regex & ex, size_t n)
{
	if(s.length() < n) return -1;

	regmatch_t match;
	int result = ex.RegExec(s.c_str()+n, 1, &match);
	if(result) return -1;
	return match.rm_so+n;
}

int
stg_indexr (string & s, const Regex & ex, size_t n)
{
	if(s.length() < n) return -1;

	regmatch_t match;
	int result = ex.RegExec(s.c_str()+n, 1, &match);
	if(result) return -1;
	return match.rm_eo+n;
}

void
stg_downcase (string & s)
{
	string::iterator stgend = s.end();
	for(string::iterator iter = s.begin(); iter != stgend; ++iter)
	{
		int c = *iter;
		if(isupper(c)) *iter = c - 'A' + 'a';
	}
}

int
stg_gsub (string &s, const char * old, const char * that)
{
	int l = strlen(old);
	int k = strlen(that);
	int i = 0;

	for(unsigned n = 0; (n = s.find(old, n)) != string::npos; i++, n += k)
	{
		s.replace(n, l, that);
	}

	return i;
}

int
stg_gsub (string &s, char old, char that)
{
	int i = 0;
	for(unsigned n = 0; (n = s.find(old, n)) != string::npos; i++, n++)
	{
		s[n] = that;
	}
	
	return i;
}
