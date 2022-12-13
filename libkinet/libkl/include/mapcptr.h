#ifndef __MAPCPTR_H__
#define __MAPCPTR_H__

typedef const char * CPTR;
struct ConstCharPtrLess : binary_function<CPTR, CPTR, bool>
{
	bool operator()(const CPTR& x, const CPTR& y) const
		{ return strcmp(x, y) < 0; }
};

typedef map<CPTR, int, ConstCharPtrLess> MapCPTR;

struct ConstCharPtrCaseLess : binary_function<CPTR, CPTR, bool>
{
	bool operator()(const CPTR& x, const CPTR& y) const
		{ return strcasecmp(x, y) < 0; }
};

typedef map<CPTR, int, ConstCharPtrCaseLess> MapCPTRCase;

#endif // __MAPCPTR_H__
